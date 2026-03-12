/**
 * ConnectionSniffer - Tarmoq ulanishlarini tinglovchi
 *
 * Linux SOCK_RAW orqali IP paketlarini ushlaydi, TCP SYN va UDP
 * ulanishlarini aniqlaydi, IP-in-IP tunnel orqali filtratorga xabar beradi.
 */

#include "sniffer.h"
#include "config.h"

#include <cstring>
#include <csignal>
#include <stdexcept>
#include <iostream>
#include <sstream>

// POSIX tarmoq
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

/* ------------------------------------------------------------------ */
/*  Statik signal yordamchi                                            */
/* ------------------------------------------------------------------ */

static ConnectionSniffer *g_sniffer_instance = nullptr;

static void signal_handler(int /*sig*/)
{
    if (g_sniffer_instance) {
        g_sniffer_instance->stop();
    }
}

/* ------------------------------------------------------------------ */
/*  Konstruktor / Destruktor                                           */
/* ------------------------------------------------------------------ */

ConnectionSniffer::ConnectionSniffer(const std::string &interface,
                                     uint16_t           port,
                                     const std::string &sniffer_ip,
                                     const std::string &filtrator_ip)
    : interface_(interface),
      port_(port),
      raw_fd_(-1),
      running_(false),
      tunnel_(sniffer_ip, filtrator_ip)
{
    create_raw_socket();

    // Global signal handler o'rnatish
    g_sniffer_instance = this;
    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);
}

ConnectionSniffer::~ConnectionSniffer()
{
    stop();
    if (raw_fd_ >= 0) {
        ::close(raw_fd_);
        raw_fd_ = -1;
    }
    g_sniffer_instance = nullptr;
}

/* ------------------------------------------------------------------ */
/*  Jamoat interfeysi                                                  */
/* ------------------------------------------------------------------ */

void ConnectionSniffer::start()
{
    running_ = true;

    std::cout << "\n"
              << "============================================================\n"
              << "  IP-in-IP Connection Sniffer\n"
              << "============================================================\n"
              << "  Interfeys : " << interface_ << "\n"
              << "  Port      : " << port_      << "\n"
              << "  To'xtatish: Ctrl+C\n"
              << "============================================================\n\n";

    uint8_t buf[MAX_PACKET_SIZE];

    while (running_) {
        ssize_t n = recvfrom(raw_fd_, buf, sizeof(buf), 0, nullptr, nullptr);
        if (n < 0) {
            if (!running_) break;   // stop() chaqirilgan
            std::perror("recvfrom");
            continue;
        }
        process_packet(buf, static_cast<int>(n));
    }

    std::cout << "[SNIFFER] To'xtatildi.\n";
    tunnel_.close();
}

void ConnectionSniffer::stop()
{
    running_ = false;
    // recvfrom ni blokdan chiqarish uchun socketni yopish
    if (raw_fd_ >= 0) {
        ::close(raw_fd_);
        raw_fd_ = -1;
    }
}

/* ------------------------------------------------------------------ */
/*  Xususiy yordamchi funksiyalar                                      */
/* ------------------------------------------------------------------ */

void ConnectionSniffer::create_raw_socket()
{
    // IPPROTO_IP — barcha IP protokollarini (TCP va UDP ni ham) qabul qilamiz
    raw_fd_ = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (raw_fd_ < 0) {
        std::perror("socket(SOCK_RAW, IPPROTO_IP)");
        throw std::runtime_error(
            "Raw socket yaratib bo'lmadi. "
            "CAP_NET_RAW imtiyozi yoki root talab qilinadi.");
    }
    std::cout << "[SNIFFER] Raw socket yaratildi (IPPROTO_IP).\n";
}

void ConnectionSniffer::process_packet(const uint8_t *buf, int len)
{
    if (len < IP_HEADER_SIZE) return;

    // ---- IP header ----
    const struct iphdr *ip = reinterpret_cast<const struct iphdr *>(buf);
    int ip_hdr_len = ip->ihl * 4;

    if (len < ip_hdr_len) return;

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
    inet_ntop(AF_INET, &ip->daddr, dst_ip, sizeof(dst_ip));

    // ---- TCP SYN ----
    if (ip->protocol == PROTO_TCP) {
        if (len < ip_hdr_len + static_cast<int>(sizeof(struct tcphdr))) return;

        const struct tcphdr *tcp =
            reinterpret_cast<const struct tcphdr *>(buf + ip_hdr_len);

        uint16_t dport = ntohs(tcp->dest);
        if (dport != port_) return;

        // Faqat SYN (yangi ulanish boshlanishi)
        if (!tcp->syn || tcp->ack) return;

        uint16_t sport = ntohs(tcp->source);
        std::string key = make_key(src_ip, sport);
        if (seen_connections_.count(key)) return;
        seen_connections_.insert(key);

        ClientConnection conn(src_ip, sport, dst_ip, dport, "TCP");
        std::cout << "[SNIFFER] Yangi TCP ulanish: " << conn.to_string() << "\n";
        tunnel_.encapsulate_and_send(conn);
    }
    // ---- UDP ----
    else if (ip->protocol == PROTO_UDP) {
        if (len < ip_hdr_len + static_cast<int>(sizeof(struct udphdr))) return;

        const struct udphdr *udp =
            reinterpret_cast<const struct udphdr *>(buf + ip_hdr_len);

        uint16_t dport = ntohs(udp->dest);
        if (dport != port_) return;

        uint16_t sport = ntohs(udp->source);
        std::string key = make_key(src_ip, sport);
        if (seen_connections_.count(key)) return;
        seen_connections_.insert(key);

        ClientConnection conn(src_ip, sport, dst_ip, dport, "UDP");
        std::cout << "[SNIFFER] Yangi UDP ulanish: " << conn.to_string() << "\n";
        tunnel_.encapsulate_and_send(conn);
    }
}

std::string ConnectionSniffer::make_key(const std::string &ip, uint16_t port)
{
    std::ostringstream oss;
    oss << ip << ":" << port;
    return oss.str();
}
