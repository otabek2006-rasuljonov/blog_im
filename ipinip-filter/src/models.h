#pragma once

/**
 * models.h - IP-in-IP tizimi uchun ma'lumot modellari va protokol tuzilmalari
 *
 * Ushbu fayl quyidagilarni o'z ichiga oladi:
 *   - IPv4 header struct (RFC 791)
 *   - TCP/UDP header structlar
 *   - ClientConnection - ushlangan ulanish haqida ma'lumot
 *   - TunnelPacket    - IP-in-IP encapsulated paket haqida ma'lumot
 */

#include <cstdint>
#include <string>
#include <chrono>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

// ----------------------------------------------------------------
// Paket header tuzilmalari
// ----------------------------------------------------------------

/**
 * IPv4 header tuzilmasi (RFC 791)
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Version|  IHL  |    TOS        |          Total Length         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Identification        |Flags|      Fragment Offset    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Time to Live |    Protocol   |         Header Checksum       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Source Address                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    Destination Address                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#pragma pack(push, 1)
struct IPv4Header {
    uint8_t  version_ihl;     ///< Version (4 bit) + IHL (4 bit)
    uint8_t  tos;             ///< Type of Service
    uint16_t total_length;    ///< Umumiy uzunlik (header + payload), network byte order
    uint16_t identification;  ///< Identifikator
    uint16_t flags_offset;    ///< Flags (3 bit) + Fragment Offset (13 bit)
    uint8_t  ttl;             ///< Time to Live
    uint8_t  protocol;        ///< Yuqori qatlam protokoli (TCP=6, UDP=17, IPIP=4)
    uint16_t checksum;        ///< Header checksum (RFC 1071)
    uint32_t src_addr;        ///< Manba IP manzil (network byte order)
    uint32_t dst_addr;        ///< Manzil IP manzil (network byte order)

    /// Version qiymatini qaytaradi (4 yoki 6)
    uint8_t version() const { return (version_ihl >> 4) & 0x0F; }

    /// Header uzunligini baytlarda qaytaradi (IHL * 4)
    uint8_t header_length() const { return (version_ihl & 0x0F) * 4; }

    /// Manba IP ni dotted-decimal formatda qaytaradi (thread-safe)
    std::string src_str() const {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src_addr, buf, sizeof(buf));
        return buf;
    }

    /// Manzil IP ni dotted-decimal formatda qaytaradi (thread-safe)
    std::string dst_str() const {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &dst_addr, buf, sizeof(buf));
        return buf;
    }
};
#pragma pack(pop)

/**
 * TCP header tuzilmasi (RFC 793)
 */
#pragma pack(push, 1)
struct TCPHeader {
    uint16_t src_port;   ///< Manba port (network byte order)
    uint16_t dst_port;   ///< Manzil port (network byte order)
    uint32_t seq_num;    ///< Ketma-ketlik raqami
    uint32_t ack_num;    ///< Tasdiqlash raqami
    uint8_t  data_off;   ///< Data offset (4 bit) + Reserved (4 bit)
    uint8_t  flags;      ///< Control flags (URG, ACK, PSH, RST, SYN, FIN)
    uint16_t window;     ///< Oyna hajmi
    uint16_t checksum;   ///< Checksum
    uint16_t urgent_ptr; ///< Urgent pointer

    /// SYN flag o'rnatilganligini tekshiradi
    bool is_syn() const { return (flags & 0x02) != 0; }

    /// ACK flag o'rnatilganligini tekshiradi
    bool is_ack() const { return (flags & 0x10) != 0; }

    /// RST flag o'rnatilganligini tekshiradi
    bool is_rst() const { return (flags & 0x04) != 0; }

    /// FIN flag o'rnatilganligini tekshiradi
    bool is_fin() const { return (flags & 0x01) != 0; }
};
#pragma pack(pop)

/**
 * UDP header tuzilmasi (RFC 768)
 */
#pragma pack(push, 1)
struct UDPHeader {
    uint16_t src_port;  ///< Manba port (network byte order)
    uint16_t dst_port;  ///< Manzil port (network byte order)
    uint16_t length;    ///< UDP uzunligi (header + data)
    uint16_t checksum;  ///< Checksum
};
#pragma pack(pop)

// ----------------------------------------------------------------
// Ilovaga xos ma'lumot tuzilmalari
// ----------------------------------------------------------------

/**
 * Ushlangan tarmoq ulanishi haqida ma'lumot.
 * Sniffer tomonidan to'ldiriladi va tunnel orqali filtratorga yuboriladi.
 */
struct ClientConnection {
    std::string client_ip;    ///< Mijoz IP manzili (dotted-decimal)
    uint16_t    client_port;  ///< Mijoz porti (host byte order)
    std::string server_ip;    ///< Server IP manzili (destination)
    uint16_t    server_port;  ///< Server porti (host byte order)
    uint8_t     protocol;     ///< Transport protokoli (PROTO_TCP yoki PROTO_UDP)

    /// Ulanish kaliti — takroriy ulanishlarni filtrlash uchun
    std::string key() const {
        return client_ip + ":" + std::to_string(client_port);
    }

    /// Insonxon o'qiladigan tavsif
    std::string to_string() const {
        std::string proto = (protocol == 6) ? "TCP" : "UDP";
        return proto + " " + client_ip + ":" + std::to_string(client_port)
             + " -> " + server_ip + ":" + std::to_string(server_port);
    }
};

/**
 * IP-in-IP tunnel orqali yuboriladigan paket metama'lumotlari.
 */
struct TunnelPacket {
    std::string outer_src;   ///< Tashqi manba IP (sniffer)
    std::string outer_dst;   ///< Tashqi manzil IP (filtrator)
    std::string inner_src;   ///< Ichki manba IP (haqiqiy mijoz)
    std::string inner_dst;   ///< Ichki manzil IP (haqiqiy server)
    uint16_t    inner_sport; ///< Ichki manba port
    uint16_t    inner_dport; ///< Ichki manzil port
    uint8_t     inner_proto; ///< Ichki transport protokoli

    std::string to_string() const {
        return "IPIP [" + outer_src + " -> " + outer_dst + "] "
             + "Inner [" + inner_src + ":" + std::to_string(inner_sport)
             + " -> " + inner_dst + ":" + std::to_string(inner_dport) + "]";
    }
};
