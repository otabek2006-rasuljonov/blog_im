#pragma once

/**
 * IPinIPTunnel - IP-in-IP tunnel moduli
 *
 * Mijoz ma'lumotlarini raw socket orqali IP-in-IP encapsulate qilib
 * filtrator serverga yuboradi.
 *
 * Foydalanish uchun root/CAP_NET_RAW huquqi talab etiladi.
 *
 * Ishlash tartibi:
 *   1. Raw socket ochiladi (SOCK_RAW, IPPROTO_RAW)
 *   2. Inner IP header yaratiladi — mijozning haqiqiy IP si
 *   3. Outer IP header yaratiladi — tunnel endpointlari
 *   4. Encapsulated paket filtratorga yuboriladi
 *
 * Paket formati:
 *   +---------------------------------+
 *   | Outer IP Header (20 bayt)       |
 *   |  src = SNIFFER_IP               |
 *   |  dst = FILTRATOR_IP             |
 *   |  proto = 4 (IP-in-IP)           |
 *   +---------------------------------+
 *   | Inner IP Header (20 bayt)       |
 *   |  src = client_ip                |
 *   |  dst = server_ip                |
 *   |  proto = 6 (TCP) / 17 (UDP)    |
 *   +---------------------------------+
 *   | Inner Payload (4 bayt)          |
 *   |  src_port (2B) | dst_port (2B)  |
 *   +---------------------------------+
 */

#include <string>
#include <cstdint>
#include "models.h"

class IPinIPTunnel {
public:
    /**
     * Tunnel yaratish.
     * @param local_ip   Sniffer IP manzili (outer src)
     * @param remote_ip  Filtrator IP manzili (outer dst)
     */
    explicit IPinIPTunnel(const std::string &local_ip,
                          const std::string &remote_ip);

    ~IPinIPTunnel();

    // Nusxa ko'chirishni taqiqlash (raw socket ownership)
    IPinIPTunnel(const IPinIPTunnel &)            = delete;
    IPinIPTunnel &operator=(const IPinIPTunnel &) = delete;

    /**
     * Mijoz ma'lumotlarini encapsulate qilib filtratorga yuborish.
     * @param conn  Mijoz ulanishi haqida ma'lumot
     * @return true  — muvaffaqiyatli yuborildi
     * @return false — xato yuz berdi
     */
    bool encapsulate_and_send(const ClientConnection &conn);

    /** Raw socketni yopish */
    void close();

private:
    std::string local_ip_;
    std::string remote_ip_;
    int         raw_fd_;     // Raw socket fayl deskriptori

    /** Raw socket yaratish (CAP_NET_RAW talab qilinadi) */
    void create_raw_socket();

    /**
     * IP header yaratish (20 bayt, RFC 791).
     * @param src_ip         Manba IP manzil (dotted-decimal)
     * @param dst_ip         Maqsad IP manzil (dotted-decimal)
     * @param proto          Protokol raqami (4 = IPIP, 6 = TCP, 17 = UDP)
     * @param payload_len    Payload uzunligi (bayt)
     * @param ttl            Time To Live (standart: 64)
     * @param[out] buf       Yozilgan header buferi (kamida 20 bayt)
     */
    static void build_ip_header(const std::string &src_ip,
                                 const std::string &dst_ip,
                                 uint8_t  proto,
                                 uint16_t payload_len,
                                 uint8_t  ttl,
                                 uint8_t  buf[20]);

    /**
     * IP header checksum hisoblash (RFC 1071).
     * @param data    Header baytlari
     * @param length  Header uzunligi (bayt)
     * @return 16-bitli one's-complement checksum
     */
    static uint16_t calculate_checksum(const uint8_t *data, size_t length);
};
