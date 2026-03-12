#pragma once

/**
 * Ma'lumot modellari - Mijoz aloqasi va tunnel paketlari
 * Data models for client connections and tunnel packets.
 */

#include <cstdint>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <vector>

/**
 * Mijoz ulanishi haqida ma'lumot
 * Holds information about a single client connection.
 */
struct ClientConnection {
    std::string client_ip;
    uint16_t    client_port;
    std::string server_ip;
    uint16_t    server_port;
    std::string protocol;   // "TCP" yoki "UDP"
    time_t      timestamp;
    std::vector<uint8_t> raw_packet;

    ClientConnection()
        : client_port(0), server_port(0), timestamp(std::time(nullptr)) {}

    ClientConnection(const std::string &c_ip, uint16_t c_port,
                     const std::string &s_ip, uint16_t s_port,
                     const std::string &proto)
        : client_ip(c_ip), client_port(c_port),
          server_ip(s_ip), server_port(s_port),
          protocol(proto), timestamp(std::time(nullptr)) {}

    std::string to_string() const {
        char buf[32];
        struct tm *t = std::localtime(&timestamp);
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);

        std::ostringstream oss;
        oss << "[" << buf << "] "
            << protocol << " "
            << client_ip << ":" << client_port
            << " -> "
            << server_ip << ":" << server_port;
        return oss.str();
    }
};

/**
 * IP-in-IP tunnel orqali yuboriladigan paket ma'lumotlari
 * Describes the encapsulated IP-in-IP tunnel packet.
 *
 * Paket tuzilishi:
 *   [Outer IP Header: sniffer -> filtrator, proto=4]
 *   [Inner IP Header: client  -> server]
 *   [Inner Payload  : src_port | dst_port]
 */
struct TunnelPacket {
    std::string outer_src;   // Sniffer IP (tunnel boshi)
    std::string outer_dst;   // Filtrator IP (tunnel oxiri)
    std::string inner_src;   // Mijoz IP (haqiqiy)
    std::string inner_dst;   // Server IP (haqiqiy)
    uint16_t    inner_sport; // Mijoz port
    uint16_t    inner_dport; // Server port
    uint8_t     protocol;    // Inner protokol (TCP=6, UDP=17)

    TunnelPacket()
        : inner_sport(0), inner_dport(0), protocol(6) {}

    std::string to_string() const {
        std::ostringstream oss;
        oss << "IPIP Tunnel: [" << outer_src << " -> " << outer_dst << "] "
            << "Inner: [" << inner_src << ":" << inner_sport
            << " -> " << inner_dst << ":" << inner_dport << "]";
        return oss.str();
    }
};
