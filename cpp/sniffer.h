#pragma once

/**
 * ConnectionSniffer - Tarmoq ulanishlarini tinglovchi
 *
 * Linux raw socket yordamida tarmoq paketlarini ushlaydi.
 * Har bir yangi TCP SYN yoki UDP ulanishi aniqlanganda,
 * mijoz IP manzilini IPinIPTunnel orqali filtratorga yuboradi.
 *
 * Foydalanish uchun root/CAP_NET_RAW huquqi talab etiladi.
 *
 * Arxitektura:
 *
 *   [Mijoz]
 *      |  TCP SYN / UDP
 *      v
 *   [ConnectionSniffer]  --IP-in-IP-->  [Filtrator]
 *      |
 *      v
 *   [Asosiy Server]
 */

#include <string>
#include <unordered_set>
#include "tunnel.h"
#include "models.h"

class ConnectionSniffer {
public:
    /**
     * Sniffer yaratish.
     * @param interface   Tinglanadigan tarmoq interfeysi (masalan "eth0")
     * @param port        Kuzatiladigan destination port
     * @param sniffer_ip  Bu mashinaning IP manzili (tunnel src)
     * @param filtrator_ip Filtrator serverning IP manzili (tunnel dst)
     */
    ConnectionSniffer(const std::string &interface,
                      uint16_t           port,
                      const std::string &sniffer_ip,
                      const std::string &filtrator_ip);

    ~ConnectionSniffer();

    // Nusxa ko'chirishni taqiqlash
    ConnectionSniffer(const ConnectionSniffer &)            = delete;
    ConnectionSniffer &operator=(const ConnectionSniffer &) = delete;

    /**
     * Snifferni ishga tushirish (bloklaydi — Ctrl+C yoki SIGTERM gacha).
     */
    void start();

    /** Snifferni to'xtatish (signal handler ichidan chaqirilishi mumkin). */
    void stop();

private:
    std::string interface_;
    uint16_t    port_;
    int         raw_fd_;
    bool        running_;

    IPinIPTunnel             tunnel_;
    std::unordered_set<std::string> seen_connections_;

    /** Raw socket yaratish */
    void create_raw_socket();

    /**
     * Bitta paketni qayta ishlash.
     * @param buf  Qabul qilingan xom bytes
     * @param len  Uzunlik
     */
    void process_packet(const uint8_t *buf, int len);

    /** Ulanish uchun unikal kalit (ip:port) */
    static std::string make_key(const std::string &ip, uint16_t port);
};
