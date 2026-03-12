#pragma once

/**
 * tunnel.h - IP-in-IP Tunnel (encapsulator)
 *
 * Bu modul ClientConnection ma'lumotlarini IP-in-IP (RFC 2003) formatida
 * encapsulate qilib, filtratorga raw socket orqali yuboradi.
 *
 * Paket tuzilmasi:
 *
 *   +-----------------------------------------+
 *   | Outer IPv4 Header                        |
 *   |  src  = SNIFFER_IP                       |
 *   |  dst  = FILTRATOR_IP                     |
 *   |  proto = 4  (IPIP)                       |
 *   +-----------------------------------------+
 *   | Inner IPv4 Header                        |
 *   |  src  = client_ip                        |
 *   |  dst  = server_ip                        |
 *   |  proto = 6/17 (TCP/UDP)                  |
 *   +-----------------------------------------+
 *   | Mini Payload: client_port | server_port  |
 *   +-----------------------------------------+
 */

#include <string>
#include <cstdint>
#include <netinet/in.h>
#include "models.h"

/**
 * IP-in-IP tunnel — mijoz ulanish ma'lumotlarini encapsulate qilib,
 * raw socket orqali filtratorga yuboradi.
 */
class IPinIPTunnel {
public:
    /**
     * @brief Konstruktor: raw socket ochadi.
     * @param local_ip  Sniffer (bu serverning) IP manzili
     * @param remote_ip Filtrator IP manzili
     * @throws std::runtime_error  socket() yoki setsockopt() muvaffaqiyatsiz bo'lsa
     */
    IPinIPTunnel(const std::string& local_ip,
                 const std::string& remote_ip);

    ~IPinIPTunnel();

    // Nusxalashni taqiqlash (raw socket resursi)
    IPinIPTunnel(const IPinIPTunnel&) = delete;
    IPinIPTunnel& operator=(const IPinIPTunnel&) = delete;

    /**
     * @brief Ulanish ma'lumotlarini IP-in-IP formatida encapsulate qilib yuboradi.
     * @param conn  Ushlangan mijoz ulanishi haqida ma'lumot
     * @return true — muvaffaqiyatli yuborildi; false — xato
     */
    bool encapsulate_and_send(const ClientConnection& conn);

private:
    std::string m_local_ip;
    std::string m_remote_ip;
    int         m_sock{-1};

    /**
     * @brief IPv4 header yasaydi va checksum hisoblab to'ldiradi.
     * @param buf             Natijaviy header yoziladigan bufer (≥20 bayt)
     * @param src             Manba IP (dotted-decimal)
     * @param dst             Manzil IP (dotted-decimal)
     * @param proto           Protokol raqami
     * @param payload_len     Payload uzunligi baytlarda (headerdan keyin)
     * @param ttl             Time-to-Live qiymati
     */
    static void build_ip_header(uint8_t*           buf,
                                 const std::string& src,
                                 const std::string& dst,
                                 uint8_t            proto,
                                 uint16_t           payload_len,
                                 uint8_t            ttl = 64);
};
