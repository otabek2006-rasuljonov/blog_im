/**
 * sniffer.cpp - Raw Socket Sniffer implementatsiyasi
 */

#include "sniffer.h"
#include "config.h"
#include "models.h"

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

// Ethernet header hajmi (baytlarda)
static constexpr int ETH_HDR_SIZE = 14;
// IPv4 minimal header hajmi
static constexpr int IP_HDR_MIN   = 20;
// TCP/UDP minimal header hajmi
static constexpr int TCP_HDR_MIN  = 20;
static constexpr int UDP_HDR_SIZE = 8;

// ----------------------------------------------------------------
// Konstruktor / Destruktor
// ----------------------------------------------------------------

ConnectionSniffer::ConnectionSniffer(const std::string& interface,
                                     uint16_t           port,
                                     const std::string& local_ip,
                                     const std::string& remote_ip)
    : m_interface(interface)
    , m_port(port)
    , m_tunnel(local_ip, remote_ip)
{
    // AF_PACKET — barcha Ethernet freymlarini qabul qilamiz
    m_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (m_sock < 0) {
        throw std::runtime_error(
            "AF_PACKET socket ochib bo'lmadi. Root huquqi kerak.");
    }

    // Faqat ko'rsatilgan interfeys paketlarini qabul qilish
    struct ifreq ifr{};
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    if (ioctl(m_sock, SIOCGIFINDEX, &ifr) < 0) {
        ::close(m_sock);
        throw std::runtime_error(
            "Interfeys topilmadi: " + interface);
    }

    struct sockaddr_ll sa{};
    sa.sll_family   = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_IP);
    sa.sll_ifindex  = ifr.ifr_ifindex;

    if (bind(m_sock, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa)) < 0) {
        ::close(m_sock);
        throw std::runtime_error("bind() muvaffaqiyatsiz: " + std::string(strerror(errno)));
    }

    std::cout << "[Sniffer] Socket ochildi — interfeys: " << interface
              << ", port: " << port << "\n";
}

ConnectionSniffer::~ConnectionSniffer() {
    if (m_sock >= 0) {
        ::close(m_sock);
    }
}

// ----------------------------------------------------------------
// Asosiy tinglash tsikli
// ----------------------------------------------------------------

void ConnectionSniffer::start() {
    std::cout << "[Sniffer] Tinglash boshlandi. To'xtatish uchun Ctrl+C.\n";
    std::cout << "[Sniffer] Filtrat port: " << m_port << "\n";

    static constexpr int BUF_SIZE = Config::RECV_BUFFER_SIZE;
    uint8_t buf[BUF_SIZE];

    while (true) {
        ssize_t len = recv(m_sock, buf, BUF_SIZE, 0);
        if (len < 0) {
            if (errno == EINTR) {
                // Signal — tinglashdan chiqish
                break;
            }
            std::cerr << "[Sniffer] recv() xato: " << strerror(errno) << "\n";
            continue;
        }
        process_packet(buf, static_cast<int>(len));
    }

    std::cout << "[Sniffer] To'xtatildi.\n";
}

// ----------------------------------------------------------------
// Paketni qayta ishlash
// ----------------------------------------------------------------

void ConnectionSniffer::process_packet(const uint8_t* buf, int len) {
    // Minimal tekshiruv: Ethernet + IP
    if (len < ETH_HDR_SIZE + IP_HDR_MIN) return;

    const uint8_t* ip_data = buf + ETH_HDR_SIZE;
    int            ip_len  = len - ETH_HDR_SIZE;

    const auto* ip_hdr = reinterpret_cast<const IPv4Header*>(ip_data);

    // Faqat IPv4 qabul qilamiz
    if (ip_hdr->version() != 4) return;

    int ip_hdr_len = ip_hdr->header_length();
    if (ip_hdr_len < IP_HDR_MIN || ip_hdr_len > ip_len) return;

    const uint8_t* transport = ip_data + ip_hdr_len;
    int            trans_len = ip_len - ip_hdr_len;

    // ---- TCP ----
    if (ip_hdr->protocol == Config::PROTO_TCP) {
        if (trans_len < TCP_HDR_MIN) return;

        const auto* tcp = reinterpret_cast<const TCPHeader*>(transport);
        uint16_t dst_port = ntohs(tcp->dst_port);

        // Faqat ko'rsatilgan portga yo'naltirilgan SYN paketlar
        if (dst_port != m_port) return;
        if (!tcp->is_syn() || tcp->is_ack()) return;

        ClientConnection conn{
            ip_hdr->src_str(),
            ntohs(tcp->src_port),
            ip_hdr->dst_str(),
            ntohs(tcp->dst_port),
            Config::PROTO_TCP
        };

        if (m_seen.count(conn.key())) return; // Takroriy ulanish
        // Hajm chegarasiga yetganda set tozalanadi
        if (m_seen.size() >= MAX_SEEN_SIZE) m_seen.clear();
        m_seen.insert(conn.key());

        std::cout << "[Sniffer] Yangi TCP ulanish: " << conn.to_string() << "\n";
        m_tunnel.encapsulate_and_send(conn);
    }
    // ---- UDP ----
    else if (ip_hdr->protocol == Config::PROTO_UDP) {
        if (trans_len < UDP_HDR_SIZE) return;

        const auto* udp = reinterpret_cast<const UDPHeader*>(transport);
        uint16_t dst_port = ntohs(udp->dst_port);

        if (dst_port != m_port) return;

        ClientConnection conn{
            ip_hdr->src_str(),
            ntohs(udp->src_port),
            ip_hdr->dst_str(),
            ntohs(udp->dst_port),
            Config::PROTO_UDP
        };

        if (m_seen.count(conn.key())) return;
        // Hajm chegarasiga yetganda set tozalanadi
        if (m_seen.size() >= MAX_SEEN_SIZE) m_seen.clear();
        m_seen.insert(conn.key());

        std::cout << "[Sniffer] Yangi UDP ulanish: " << conn.to_string() << "\n";
        m_tunnel.encapsulate_and_send(conn);
    }
}
