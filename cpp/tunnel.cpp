/**
 * IPinIPTunnel - IP-in-IP tunnel implementation
 *
 * Mijoz IP manzilini raw socket yordamida IP-in-IP encapsulate qilib
 * filtrator serverga yuboradi.
 *
 * Zarur imtiyozlar: CAP_NET_RAW (yoki root)
 */

#include "tunnel.h"
#include "config.h"

#include <atomic>
#include <cstring>
#include <stdexcept>
#include <iostream>

// POSIX / Linux tarmoq sarlavhalari
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* ------------------------------------------------------------------ */
/*  Konstruktor / Destruktor                                           */
/* ------------------------------------------------------------------ */

IPinIPTunnel::IPinIPTunnel(const std::string &local_ip,
                           const std::string &remote_ip)
    : local_ip_(local_ip), remote_ip_(remote_ip), raw_fd_(-1)
{
    create_raw_socket();
}

IPinIPTunnel::~IPinIPTunnel()
{
    close();
}

/* ------------------------------------------------------------------ */
/*  Jamoat interfeysi                                                  */
/* ------------------------------------------------------------------ */

bool IPinIPTunnel::encapsulate_and_send(const ClientConnection &conn)
{
    /*
     * Paket tuzilishi (baytlar soni):
     *   Outer IP header  : 20
     *   Inner IP header  : 20
     *   Inner payload    :  4  (src_port | dst_port)
     *   Jami             : 44
     */

    // --- Inner payload: port juftligi -----------------------------------
    uint8_t inner_payload[4];
    uint16_t sport_n = htons(conn.client_port);
    uint16_t dport_n = htons(conn.server_port);
    std::memcpy(inner_payload,     &sport_n, 2);
    std::memcpy(inner_payload + 2, &dport_n, 2);

    // --- Inner IP header ------------------------------------------------
    uint8_t inner_proto = (conn.protocol == "UDP") ? PROTO_UDP : PROTO_TCP;
    uint8_t inner_hdr[IP_HEADER_SIZE];
    build_ip_header(conn.client_ip, conn.server_ip,
                    inner_proto,
                    static_cast<uint16_t>(sizeof(inner_payload)),
                    64, inner_hdr);

    // --- Outer IP header ------------------------------------------------
    // outer payload = inner IP header + inner payload
    uint16_t inner_len = static_cast<uint16_t>(IP_HEADER_SIZE + sizeof(inner_payload));
    uint8_t outer_hdr[IP_HEADER_SIZE];
    build_ip_header(local_ip_, remote_ip_,
                    PROTO_IPIP,
                    inner_len,
                    128, outer_hdr);

    // --- To'liq paket yig'ish -------------------------------------------
    uint8_t packet[IP_HEADER_SIZE * 2 + sizeof(inner_payload)];
    std::memcpy(packet,                          outer_hdr,     IP_HEADER_SIZE);
    std::memcpy(packet + IP_HEADER_SIZE,         inner_hdr,     IP_HEADER_SIZE);
    std::memcpy(packet + IP_HEADER_SIZE * 2,     inner_payload, sizeof(inner_payload));

    // --- Yuborish -------------------------------------------------------
    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(remote_ip_.c_str());

    ssize_t sent = sendto(raw_fd_,
                          packet, sizeof(packet),
                          0,
                          reinterpret_cast<struct sockaddr *>(&dest),
                          sizeof(dest));

    if (sent < 0) {
        std::perror("sendto");
        return false;
    }

    // Log uchun tunnel ma'lumotlarini chiqarish
    TunnelPacket tp;
    tp.outer_src   = local_ip_;
    tp.outer_dst   = remote_ip_;
    tp.inner_src   = conn.client_ip;
    tp.inner_dst   = conn.server_ip;
    tp.inner_sport = conn.client_port;
    tp.inner_dport = conn.server_port;
    tp.protocol    = inner_proto;
    std::cout << "[TUNNEL] Yuborildi: " << tp.to_string() << "\n";

    return true;
}

void IPinIPTunnel::close()
{
    if (raw_fd_ >= 0) {
        ::close(raw_fd_);
        raw_fd_ = -1;
        std::cout << "[TUNNEL] Socket yopildi.\n";
    }
}

/* ------------------------------------------------------------------ */
/*  Xususiy yordamchi funksiyalar                                      */
/* ------------------------------------------------------------------ */

void IPinIPTunnel::create_raw_socket()
{
    // IPPROTO_RAW — IP_HDRINCL avtomatik yoqiladi, biz header yozamiz
    raw_fd_ = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (raw_fd_ < 0) {
        std::perror("socket(IPPROTO_RAW)");
        throw std::runtime_error(
            "Raw socket yaratib bo'lmadi. "
            "CAP_NET_RAW imtiyozi yoki root talab qilinadi.");
    }

    // IP_HDRINCL: biz o'zimiz IP headerni taqdim etamiz
    int opt = 1;
    if (setsockopt(raw_fd_, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) {
        std::perror("setsockopt(IP_HDRINCL)");
        ::close(raw_fd_);
        raw_fd_ = -1;
        throw std::runtime_error("IP_HDRINCL sozlab bo'lmadi.");
    }

    std::cout << "[TUNNEL] Raw socket yaratildi: "
              << local_ip_ << " -> " << remote_ip_ << "\n";
}

void IPinIPTunnel::build_ip_header(const std::string &src_ip,
                                    const std::string &dst_ip,
                                    uint8_t  proto,
                                    uint16_t payload_len,
                                    uint8_t  ttl,
                                    uint8_t  buf[20])
{
    /*
     * IP Header formati (RFC 791), har biri 32-bit so'z:
     *
     *  0               1               2               3
     *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |Ver| IHL |  TOS  |          Total Length                       |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |         Identification        |Flags|    Fragment Offset      |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |  TTL  |    Protocol           |        Header Checksum        |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                       Source Address                          |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |                    Destination Address                        |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     */

    std::memset(buf, 0, IP_HEADER_SIZE);

    uint16_t total_len = htons(static_cast<uint16_t>(IP_HEADER_SIZE + payload_len));
    // Paket identifikatori: atomik hisoblagich, har paket uchun unikal (RFC 791)
    static std::atomic<uint16_t> pkt_id{1};
    uint16_t id        = htons(pkt_id.fetch_add(1, std::memory_order_relaxed));
    uint16_t frag_off  = htons(0x4000); // Don't Fragment

    buf[0]  = (4 << 4) | 5;                 // Version=4, IHL=5 (20 bayt)
    buf[1]  = 0;                             // TOS
    buf[2]  = static_cast<uint8_t>(total_len >> 8);
    buf[3]  = static_cast<uint8_t>(total_len & 0xFF);
    buf[4]  = static_cast<uint8_t>(id >> 8);
    buf[5]  = static_cast<uint8_t>(id & 0xFF);
    buf[6]  = static_cast<uint8_t>(frag_off >> 8);
    buf[7]  = static_cast<uint8_t>(frag_off & 0xFF);
    buf[8]  = ttl;
    buf[9]  = proto;
    buf[10] = 0; buf[11] = 0;               // Checksum (hozircha 0)

    // Manba IP
    struct in_addr src_addr{};
    inet_aton(src_ip.c_str(), &src_addr);
    std::memcpy(buf + 12, &src_addr.s_addr, 4);

    // Maqsad IP
    struct in_addr dst_addr{};
    inet_aton(dst_ip.c_str(), &dst_addr);
    std::memcpy(buf + 16, &dst_addr.s_addr, 4);

    // Checksum hisoblash va joylashtirish
    uint16_t cksum = calculate_checksum(buf, IP_HEADER_SIZE);
    buf[10] = static_cast<uint8_t>(cksum >> 8);
    buf[11] = static_cast<uint8_t>(cksum & 0xFF);
}

uint16_t IPinIPTunnel::calculate_checksum(const uint8_t *data, size_t length)
{
    /*
     * RFC 1071 — 16-bitli one's complement yig'indisi.
     * Agar uzunlik toq bo'lsa, oxirgi baytga 0 qo'shiladi.
     */
    uint32_t sum = 0;
    const uint16_t *ptr = reinterpret_cast<const uint16_t *>(data);

    while (length > 1) {
        sum    += *ptr++;
        length -= 2;
    }
    if (length == 1) {
        sum += *reinterpret_cast<const uint8_t *>(ptr);
    }

    // Carry bitlarni aylantirish
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return static_cast<uint16_t>(~sum);
}
