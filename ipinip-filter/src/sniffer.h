#pragma once

/**
 * sniffer.h - Tarmoq ulanishlarini tinglash (Raw Socket Sniffer)
 *
 * Bu modul:
 *   1. Raw AF_PACKET socket orqali barcha kiruvchi paketlarni ushlaydi.
 *   2. Faqat belgilangan portga yo'naltirilgan TCP SYN va UDP paketlarni
 *      filtrlaydi.
 *   3. Har bir yangi ulanish uchun IPinIPTunnel::encapsulate_and_send()
 *      ni chaqiradi.
 *
 * Ishlatish:
 *   ConnectionSniffer sniffer("eth0", 8080);
 *   sniffer.start();   // bloklovchi tsikl
 */

#include <string>
#include <unordered_set>
#include "tunnel.h"

/**
 * Raw socket yordamida tarmoq paketlarini tinglaydigan sinf.
 */
class ConnectionSniffer {
public:
    /**
     * @param interface   Tinglanadigan tarmoq interfeysi (masalan, "eth0")
     * @param port        Filtrlash uchun manzil port
     * @param local_ip    Sniffer IP manzili (tunnel manba)
     * @param remote_ip   Filtrator IP manzili (tunnel manzil)
     */
    ConnectionSniffer(const std::string& interface,
                      uint16_t           port,
                      const std::string& local_ip,
                      const std::string& remote_ip);

    ~ConnectionSniffer();

    // Nusxalashni taqiqlash
    ConnectionSniffer(const ConnectionSniffer&) = delete;
    ConnectionSniffer& operator=(const ConnectionSniffer&) = delete;

    /**
     * @brief Paketlarni qabul qilish tsiklini boshlaydi (bloklovchi).
     *        SIGINT (Ctrl+C) signali bilan to'xtatiladi.
     */
    void start();

private:
    std::string m_interface;
    uint16_t    m_port;
    int         m_sock{-1};
    IPinIPTunnel m_tunnel;

    /// Bir sessiyada allaqachon qayta ishlangan ulanishlar kalitlari.
    /// Hajmi MAX_SEEN_SIZE ga yetganda tozalanadi.
    std::unordered_set<std::string> m_seen;

    /// m_seen uchun maksimal yozuv soni (xotirani cheklash)
    static constexpr std::size_t MAX_SEEN_SIZE = 65536;

    /**
     * @brief Bitta qabul qilingan paketni qayta ishlaydi.
     * @param buf   Paket ma'lumotlari buferi
     * @param len   Buferdagi baytlar soni
     */
    void process_packet(const uint8_t* buf, int len);
};
