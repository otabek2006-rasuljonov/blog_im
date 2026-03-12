#pragma once

/**
 * Filtrator - IP-in-IP paketlarni qabul qilib tekshiruvchi
 *
 * Raw socket orqali IP-in-IP (protokol 4) paketlarni qabul qiladi,
 * inner IP ma'lumotlarini ajratib oladi va qoidalar asosida
 * ruxsat berish / bloklash qarorini qabul qiladi.
 *
 * Qoidalar tizimi (FilterRule):
 *   - ALLOW  : ruxsat berish
 *   - DENY   : bloklash
 *
 * Qoida maydonlari:
 *   - client_ip    : mijoz IP (bo'sh = istalgan)
 *   - server_port  : server port (0 = istalgan)
 *   - protocol     : protokol (0 = istalgan, 6=TCP, 17=UDP)
 */

#include <cstdint>
#include <string>
#include <vector>

/** Bitta filtr qoidasi */
struct FilterRule {
    enum Action { ALLOW, DENY } action;

    std::string client_ip;    // Mijoz IP filtri (bo'sh = istalgan)
    uint16_t    server_port;  // Server port filtri (0 = istalgan)
    uint8_t     protocol;     // Protokol filtri    (0 = istalgan)
    std::string description;  // Tavsif (log uchun)

    FilterRule(Action a,
               const std::string &ip   = "",
               uint16_t           port = 0,
               uint8_t            proto = 0,
               const std::string &desc = "")
        : action(a), client_ip(ip), server_port(port),
          protocol(proto), description(desc) {}

    /** Bu qoida berilgan so'rovga mos keladimi? */
    bool matches(const std::string &c_ip,
                 uint16_t           s_port,
                 uint8_t            proto) const;
};

/** Filtratorning asosiy klassi */
class Filtrator {
public:
    /**
     * Filtrator yaratish.
     * @param listen_ip  Qaysi interfeysdagi IP-in-IP paketlarini tinglash
     */
    explicit Filtrator(const std::string &listen_ip);
    ~Filtrator();

    // Nusxa ko'chirishni taqiqlash
    Filtrator(const Filtrator &)            = delete;
    Filtrator &operator=(const Filtrator &) = delete;

    /** Qoida qo'shish (first-match — qo'shilish tartibida tekshiriladi) */
    void add_rule(const FilterRule &rule);

    /**
     * Paketlarni tinglashni boshlash (bloklaydi — Ctrl+C gacha).
     */
    void start_listening();

    /** Filtratoni to'xtatish */
    void stop();

private:
    std::string          listen_ip_;
    int                  raw_fd_;
    bool                 running_;
    std::vector<FilterRule> rules_;

    void create_raw_socket();

    /**
     * Kelgan xom paketni qayta ishlash (decapsulate + tekshirish).
     * @param buf  Qabul qilingan baytlar
     * @param len  Uzunlik
     */
    void process_ipip_packet(const uint8_t *buf, int len);

    /**
     * Mijoz ma'lumotlarini qoidalar asosida tekshirish.
     * @return true — ruxsat berildi, false — bloklandi
     */
    bool check_rules(const std::string &client_ip,
                     uint16_t           server_port,
                     uint8_t            proto) const;
};
