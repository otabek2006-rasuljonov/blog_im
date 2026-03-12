#pragma once

/**
 * filtrator.h - IP-in-IP Filtrator
 *
 * Bu modul sniffer tomonidan yuborilgan IP-in-IP paketlarni:
 *   1. Qabul qiladi (raw socket, IPPROTO_RAW yoki custom protocol)
 *   2. Tashqi (outer) IP headerni o'qiydi
 *   3. Ichki (inner) IP headerni ajratib, mijoz IP va portini oladi
 *   4. Qoidalar (blacklist / whitelist) asosida ruxsat yoki bloklash qaytaradi
 *
 * Qoidalar FilterRule struct sifatida add_rule() orqali qo'shiladi.
 * Birinchi mos qoida ishlatiladi (first-match).
 */

#include <string>
#include <vector>
#include <cstdint>

/**
 * Filtr qoidasi.
 *
 * ip_pattern — to'liq manzil ("192.168.1.10") yoki
 *              prefix sifatida ishlatiladigan qisman manzil
 *              ("192.168.1." — prefiks tekshiruv).
 * action     — true = ruxsat bering (allow), false = bloklang (deny)
 */
struct FilterRule {
    std::string ip_pattern; ///< IP yoki prefiks
    bool        allow;      ///< true=allow, false=deny
};

/**
 * Filtrat natijasi
 */
enum class FilterResult {
    Allow,  ///< Ulanishga ruxsat berildi
    Deny,   ///< Ulanish bloklandi
};

/**
 * IP-in-IP paketlarni qabul qilib, filtrlash qoidalarini qo'llaydigan sinf.
 */
class Filtrator {
public:
    /**
     * @param listen_ip   Filtrator IP manzili (qabul qiladigan interfeys)
     */
    explicit Filtrator(const std::string& listen_ip);
    ~Filtrator();

    // Nusxalashni taqiqlash
    Filtrator(const Filtrator&) = delete;
    Filtrator& operator=(const Filtrator&) = delete;

    /**
     * @brief Filtr qoidasini qo'shadi (first-match tartibi).
     * @param rule  Qo'shiladigan qoida
     */
    void add_rule(const FilterRule& rule);

    /**
     * @brief IP-in-IP paketlarni tinglovchi tsiklni boshlaydi (bloklovchi).
     *        SIGINT (Ctrl+C) bilan to'xtatiladi.
     */
    void start();

private:
    std::string              m_listen_ip;
    int                      m_sock{-1};
    std::vector<FilterRule>  m_rules;

    /**
     * @brief Bitta qabul qilingan IP-in-IP paketni qayta ishlaydi.
     * @param buf  Paket buferi (outer IP headerdan boshlanadi)
     * @param len  Bufer uzunligi (baytlarda)
     */
    void process_ipip_packet(const uint8_t* buf, int len);

    /**
     * @brief Berilgan IP manziliga qoidalarni qo'llaydi.
     * @param client_ip  Tekshiriladigan mijoz IP
     * @return FilterResult::Allow yoki FilterResult::Deny
     */
    FilterResult apply_rules(const std::string& client_ip) const;
};
