/**
 * tunnel.cpp - IP-in-IP Tunnel implementatsiyasi
 */

#include "tunnel.h"
#include "checksum.h"
#include "config.h"
#include "models.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <stdexcept>
#include <iostream>

// IPv4 header hajmi (baytlarda, options yo'q)
static constexpr int IP_HEADER_SIZE = 20;

// IP identification counter — har paketda ortib boradi
static std::atomic<uint16_t> g_ip_id{1};

// ----------------------------------------------------------------
// Konstruktor / Destruktor
// ----------------------------------------------------------------

IPinIPTunnel::IPinIPTunnel(const std::string& local_ip,
                           const std::string& remote_ip)
    : m_local_ip(local_ip), m_remote_ip(remote_ip)
{
    // Raw socket: IPPROTO_RAW — IP headerni o'zimiz to'ldiramiz
    m_sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (m_sock < 0) {
        throw std::runtime_error(
            "Raw socket ochib bo'lmadi. Root huquqi kerak (sudo).");
    }

    // IP_HDRINCL: yadro IP headerini qo'shmaydi — biz o'zimiz yozamiz
    int opt = 1;
    if (setsockopt(m_sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) {
        ::close(m_sock);
        throw std::runtime_error("setsockopt(IP_HDRINCL) muvaffaqiyatsiz bo'ldi.");
    }

    std::cout << "[Tunnel] Raw socket ochildi: "
              << m_local_ip << " -> " << m_remote_ip << "\n";
}

IPinIPTunnel::~IPinIPTunnel() {
    if (m_sock >= 0) {
        ::close(m_sock);
        std::cout << "[Tunnel] Socket yopildi.\n";
    }
}

// ----------------------------------------------------------------
// IP Header yasash
// ----------------------------------------------------------------

void IPinIPTunnel::build_ip_header(uint8_t*           buf,
                                    const std::string& src,
                                    const std::string& dst,
                                    uint8_t            proto,
                                    uint16_t           payload_len,
                                    uint8_t            ttl)
{
    auto* hdr = reinterpret_cast<IPv4Header*>(buf);
    std::memset(hdr, 0, IP_HEADER_SIZE);

    hdr->version_ihl   = (4 << 4) | 5;          // IPv4, IHL=5 (20 bayt)
    hdr->tos           = 0;
    hdr->total_length  = htons(static_cast<uint16_t>(IP_HEADER_SIZE + payload_len));
    hdr->identification = htons(g_ip_id.fetch_add(1, std::memory_order_relaxed));
    hdr->flags_offset  = htons(0x4000);           // Don't Fragment
    hdr->ttl           = ttl;
    hdr->protocol      = proto;
    hdr->checksum      = 0;                       // Avval 0, so'ng hisoblaymiz
    hdr->src_addr      = inet_addr(src.c_str());
    hdr->dst_addr      = inet_addr(dst.c_str());

    // Checksum hisoblash
    hdr->checksum = htons(compute_checksum(buf, IP_HEADER_SIZE));
}

// ----------------------------------------------------------------
// Encapsulate & Send
// ----------------------------------------------------------------

bool IPinIPTunnel::encapsulate_and_send(const ClientConnection& conn) {
    //
    // Paket tuzilmasi (baytlarda):
    //
    //  [Outer IP Header : 20]
    //  [Inner IP Header : 20]
    //  [Mini payload    :  4]   <- client_port (2) | server_port (2)
    //
    static constexpr int MINI_PAYLOAD_SIZE = 4;
    static constexpr int INNER_PACKET_SIZE = IP_HEADER_SIZE + MINI_PAYLOAD_SIZE;
    static constexpr int TOTAL_SIZE        = IP_HEADER_SIZE + INNER_PACKET_SIZE;

    uint8_t packet[TOTAL_SIZE];
    std::memset(packet, 0, TOTAL_SIZE);

    // ---- 1. Inner IP header (mijoz -> server) ----
    build_ip_header(
        packet + IP_HEADER_SIZE,          // inner header offset
        conn.client_ip,
        conn.server_ip,
        conn.protocol,                    // TCP=6 yoki UDP=17
        MINI_PAYLOAD_SIZE,
        64
    );

    // ---- 2. Mini payload: port ma'lumotlari ----
    uint8_t* payload_ptr = packet + IP_HEADER_SIZE + IP_HEADER_SIZE;
    uint16_t sport_net   = htons(conn.client_port);
    uint16_t dport_net   = htons(conn.server_port);
    std::memcpy(payload_ptr,     &sport_net, 2);
    std::memcpy(payload_ptr + 2, &dport_net, 2);

    // ---- 3. Outer IP header (sniffer -> filtrator) ----
    build_ip_header(
        packet,
        m_local_ip,
        m_remote_ip,
        Config::PROTO_IPIP,               // 4 = IP-in-IP
        INNER_PACKET_SIZE,
        128
    );

    // ---- 4. Filtratorga yuborish ----
    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(m_remote_ip.c_str());

    ssize_t sent = sendto(m_sock,
                          packet,
                          TOTAL_SIZE,
                          0,
                          reinterpret_cast<struct sockaddr*>(&dest),
                          sizeof(dest));
    if (sent < 0) {
        std::cerr << "[Tunnel] sendto() xato: " << strerror(errno) << "\n";
        return false;
    }

    TunnelPacket tp{
        m_local_ip,  m_remote_ip,
        conn.client_ip, conn.server_ip,
        conn.client_port, conn.server_port,
        conn.protocol
    };
    std::cout << "[Tunnel] Yuborildi: " << tp.to_string() << "\n";
    return true;
}
