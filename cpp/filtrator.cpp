/**
 * Filtrator - IP-in-IP paketlarni qabul qilib tekshiruvchi
 *
 * Outer IP headerini ajratib tashlab inner IP paket ma'lumotlarini
 * o'qiydi; keyin qoidalar ro'yxatiga moslik tekshiriladi.
 */

#include "filtrator.h"
#include "config.h"

#include <cstring>
#include <csignal>
#include <stdexcept>
#include <iostream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* ------------------------------------------------------------------ */
/*  FilterRule::matches                                                */
/* ------------------------------------------------------------------ */

bool FilterRule::matches(const std::string &c_ip,
                          uint16_t           s_port,
                          uint8_t            proto) const
{
    if (!client_ip.empty()  && client_ip   != c_ip)   return false;
    if (server_port != 0    && server_port != s_port)  return false;
    if (protocol    != 0    && protocol    != proto)   return false;
    return true;
}

/* ------------------------------------------------------------------ */
/*  Statik signal yordamchi                                            */
/* ------------------------------------------------------------------ */

static Filtrator *g_filtrator_instance = nullptr;

static void filtrator_signal_handler(int /*sig*/)
{
    if (g_filtrator_instance) {
        g_filtrator_instance->stop();
    }
}

/* ------------------------------------------------------------------ */
/*  Konstruktor / Destruktor                                           */
/* ------------------------------------------------------------------ */

Filtrator::Filtrator(const std::string &listen_ip)
    : listen_ip_(listen_ip), raw_fd_(-1), running_(false)
{
    create_raw_socket();

    g_filtrator_instance = this;
    std::signal(SIGINT,  filtrator_signal_handler);
    std::signal(SIGTERM, filtrator_signal_handler);
}

Filtrator::~Filtrator()
{
    stop();
    g_filtrator_instance = nullptr;
}

/* ------------------------------------------------------------------ */
/*  Jamoat interfeysi                                                  */
/* ------------------------------------------------------------------ */

void Filtrator::add_rule(const FilterRule &rule)
{
    rules_.push_back(rule);
    std::cout << "[FILTRATOR] Qoida qo'shildi: "
              << (rule.action == FilterRule::ALLOW ? "ALLOW" : "DENY")
              << " — " << rule.description << "\n";
}

void Filtrator::start_listening()
{
    running_ = true;

    std::cout << "\n"
              << "============================================================\n"
              << "  IP-in-IP Filtrator\n"
              << "============================================================\n"
              << "  Tinglash IP : " << listen_ip_ << "\n"
              << "  Qoidalar    : " << rules_.size() << " ta\n"
              << "  To'xtatish  : Ctrl+C\n"
              << "============================================================\n\n";

    uint8_t buf[MAX_PACKET_SIZE];

    while (running_) {
        ssize_t n = recvfrom(raw_fd_, buf, sizeof(buf), 0, nullptr, nullptr);
        if (n < 0) {
            if (!running_) break;
            std::perror("recvfrom");
            continue;
        }
        process_ipip_packet(buf, static_cast<int>(n));
    }

    std::cout << "[FILTRATOR] To'xtatildi.\n";
}

void Filtrator::stop()
{
    running_ = false;
    if (raw_fd_ >= 0) {
        ::close(raw_fd_);
        raw_fd_ = -1;
    }
}

/* ------------------------------------------------------------------ */
/*  Xususiy yordamchi funksiyalar                                      */
/* ------------------------------------------------------------------ */

void Filtrator::create_raw_socket()
{
    // PROTO_IPIP = 4 — faqat IP-in-IP paketlarni qabul qilamiz
    raw_fd_ = socket(AF_INET, SOCK_RAW, PROTO_IPIP);
    if (raw_fd_ < 0) {
        std::perror("socket(SOCK_RAW, PROTO_IPIP)");
        throw std::runtime_error(
            "Raw socket yaratib bo'lmadi. "
            "CAP_NET_RAW imtiyozi yoki root talab qilinadi.");
    }
    std::cout << "[FILTRATOR] Raw socket yaratildi (PROTO_IPIP=4).\n";
}

void Filtrator::process_ipip_packet(const uint8_t *buf, int len)
{
    /*
     * Paket tuzilishi:
     *   [Outer IP header]  — ajratib tashlanadi
     *   [Inner IP header]  — mijoz IP, server IP, protokol
     *   [Inner payload]    — src_port (2B) | dst_port (2B)
     */

    if (len < IP_HEADER_SIZE) return;

    // Outer IP header uzunligi
    int outer_ihl = (buf[0] & 0x0F) * 4;
    if (len < outer_ihl + IP_HEADER_SIZE) {
        std::cerr << "[FILTRATOR] Paket juda kichik: " << len << " bayt\n";
        return;
    }

    // Inner IP header
    const uint8_t *inner = buf + outer_ihl;
    int  inner_ihl = (inner[0] & 0x0F) * 4;

    char inner_src[INET_ADDRSTRLEN];
    char inner_dst[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, inner + 12, inner_src, sizeof(inner_src));
    inet_ntop(AF_INET, inner + 16, inner_dst, sizeof(inner_dst));

    uint8_t  inner_proto = inner[9];

    // Port ma'lumotlari (inner payloaddan)
    uint16_t src_port = 0, dst_port = 0;
    const uint8_t *payload = inner + inner_ihl;
    int remaining = len - outer_ihl - inner_ihl;
    if (remaining >= 4) {
        src_port = static_cast<uint16_t>((payload[0] << 8) | payload[1]);
        dst_port = static_cast<uint16_t>((payload[2] << 8) | payload[3]);
    }

    std::cout << "[FILTRATOR] Qabul qilindi — Mijoz: "
              << inner_src << ":" << src_port
              << " -> Server: " << inner_dst << ":" << dst_port
              << " (Proto: " << static_cast<int>(inner_proto) << ")\n";

    bool allowed = check_rules(inner_src, dst_port, inner_proto);
    std::cout << "[FILTRATOR] Qaror: "
              << (allowed ? "✅ RUXSAT" : "🚫 BLOKLANDI") << "\n\n";
}

bool Filtrator::check_rules(const std::string &client_ip,
                              uint16_t           server_port,
                              uint8_t            proto) const
{
    // Qoidalar ro'yxatini boshlangichdan tekshirish (first-match)
    for (const auto &rule : rules_) {
        if (rule.matches(client_ip, server_port, proto)) {
            std::cout << "[FILTRATOR] Mos qoida: " << rule.description << "\n";
            return rule.action == FilterRule::ALLOW;
        }
    }
    // Standart: hech qoida mos kelmasa — ruxsat berish
    return true;
}
