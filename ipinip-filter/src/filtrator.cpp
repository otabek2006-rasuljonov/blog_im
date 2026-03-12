/**
 * filtrator.cpp - IP-in-IP Filtrator implementatsiyasi
 */

#include "filtrator.h"
#include "config.h"
#include "models.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

static constexpr int IP_HDR_MIN = 20;

// ----------------------------------------------------------------
// Konstruktor / Destruktor
// ----------------------------------------------------------------

Filtrator::Filtrator(const std::string& listen_ip)
    : m_listen_ip(listen_ip)
{
    // IP-in-IP (protokol 4) paketlarini qabul qilish uchun raw socket
    m_sock = socket(AF_INET, SOCK_RAW, Config::PROTO_IPIP);
    if (m_sock < 0) {
        throw std::runtime_error(
            "Raw socket (PROTO_IPIP) ochib bo'lmadi. Root huquqi kerak.");
    }

    // Faqat ko'rsatilgan IP manzilga kelgan paketlar
    struct sockaddr_in sa{};
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = inet_addr(listen_ip.c_str());
    if (bind(m_sock, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa)) < 0) {
        ::close(m_sock);
        throw std::runtime_error(
            "bind() muvaffaqiyatsiz: " + std::string(strerror(errno)));
    }

    std::cout << "[Filtrator] Socket ochildi: " << listen_ip << "\n";
}

Filtrator::~Filtrator() {
    if (m_sock >= 0) {
        ::close(m_sock);
    }
}

// ----------------------------------------------------------------
// Qoida boshqaruvi
// ----------------------------------------------------------------

void Filtrator::add_rule(const FilterRule& rule) {
    m_rules.push_back(rule);
    std::cout << "[Filtrator] Qoida qo'shildi: "
              << rule.ip_pattern
              << " -> " << (rule.allow ? "ALLOW" : "DENY") << "\n";
}

// ----------------------------------------------------------------
// Asosiy tinglash tsikli
// ----------------------------------------------------------------

void Filtrator::start() {
    std::cout << "[Filtrator] IP-in-IP paketlar kutilmoqda...\n";

    uint8_t buf[Config::RECV_BUFFER_SIZE];

    while (true) {
        struct sockaddr_in from{};
        socklen_t          from_len = sizeof(from);

        ssize_t len = recvfrom(m_sock,
                               buf,
                               sizeof(buf),
                               0,
                               reinterpret_cast<struct sockaddr*>(&from),
                               &from_len);
        if (len < 0) {
            if (errno == EINTR) break; // Signal
            std::cerr << "[Filtrator] recvfrom() xato: " << strerror(errno) << "\n";
            continue;
        }

        process_ipip_packet(buf, static_cast<int>(len));
    }

    std::cout << "[Filtrator] To'xtatildi.\n";
}

// ----------------------------------------------------------------
// IP-in-IP paketni ochish (decapsulate)
// ----------------------------------------------------------------

void Filtrator::process_ipip_packet(const uint8_t* buf, int len) {
    // Linux raw socket qabul qilganda outer IP header ham keladi
    if (len < IP_HDR_MIN) return;

    const auto* outer = reinterpret_cast<const IPv4Header*>(buf);
    if (outer->version() != 4) return;

    int outer_hdr_len = outer->header_length();
    if (outer_hdr_len < IP_HDR_MIN || outer_hdr_len >= len) return;

    // Outer protocol IP-in-IP (4) bo'lishi kerak
    if (outer->protocol != Config::PROTO_IPIP) return;

    // ---- Inner paket ----
    const uint8_t* inner_data = buf + outer_hdr_len;
    int            inner_len  = len - outer_hdr_len;

    if (inner_len < IP_HDR_MIN) {
        std::cerr << "[Filtrator] Inner paket juda kichik: "
                  << inner_len << " bayt\n";
        return;
    }

    const auto* inner = reinterpret_cast<const IPv4Header*>(inner_data);
    if (inner->version() != 4) return;

    int inner_hdr_len = inner->header_length();
    if (inner_hdr_len < IP_HDR_MIN) return;

    std::string client_ip  = inner->src_str();
    std::string server_ip  = inner->dst_str();
    uint8_t     inner_proto = inner->protocol;

    // ---- Port ma'lumotlari (mini payload) ----
    uint16_t client_port = 0;
    uint16_t server_port = 0;
    const uint8_t* payload = inner_data + inner_hdr_len;
    int payload_len = inner_len - inner_hdr_len;
    if (payload_len >= 4) {
        std::memcpy(&client_port, payload,     2); client_port = ntohs(client_port);
        std::memcpy(&server_port, payload + 2, 2); server_port = ntohs(server_port);
    }

    std::string proto_name = (inner_proto == Config::PROTO_TCP) ? "TCP" : "UDP";
    std::cout << "[Filtrator] Qabul: " << proto_name
              << " " << client_ip << ":" << client_port
              << " -> " << server_ip << ":" << server_port << "\n";

    // ---- Qoidalarni qo'llash ----
    FilterResult result = apply_rules(client_ip);
    if (result == FilterResult::Allow) {
        std::cout << "[Filtrator] RUXSAT: " << client_ip << "\n";
    } else {
        std::cout << "[Filtrator] BLOKLANDI: " << client_ip << "\n";
    }
}

// ----------------------------------------------------------------
// Filtr qoidalari (first-match)
// ----------------------------------------------------------------

FilterResult Filtrator::apply_rules(const std::string& client_ip) const {
    // Parse client IP into binary form for accurate comparison
    struct in_addr client_addr{};
    if (inet_pton(AF_INET, client_ip.c_str(), &client_addr) != 1) {
        // Geçersiz IP — bloklash
        return FilterResult::Deny;
    }

    for (const auto& rule : m_rules) {
        // Agar qoida to'liq IP manzil bo'lsa — aniq taqqoslash
        struct in_addr rule_addr{};
        if (inet_pton(AF_INET, rule.ip_pattern.c_str(), &rule_addr) == 1) {
            if (client_addr.s_addr == rule_addr.s_addr) {
                return rule.allow ? FilterResult::Allow : FilterResult::Deny;
            }
            continue;
        }

        // Aks holda — qoida CIDR prefix sifatida talqin qilinadi (masalan, "10.0.")
        // Faqat to'liq oktet chegarasida prefix tekshiruvi amalga oshiriladi.
        // Buning uchun prefiks oxirgi "." bilan tugashi kerak.
        const std::string& pat = rule.ip_pattern;
        if (!pat.empty() && pat.back() == '.') {
            if (client_ip.size() > pat.size() &&
                client_ip.compare(0, pat.size(), pat) == 0)
            {
                return rule.allow ? FilterResult::Allow : FilterResult::Deny;
            }
        }
    }
    // Hech qanday qoida mos kelmasa — default allow
    return FilterResult::Allow;
}
