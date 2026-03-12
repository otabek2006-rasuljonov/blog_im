/**
 * filtrator_main.cpp — Filtrator dasturi kirish nuqtasi
 *
 * Ishga tushirish:
 *   sudo ./filtrator
 */

#include "filtrator.h"
#include "config.h"
#include <iostream>

int main()
{
    std::cout << R"(
    ╔══════════════════════════════════════════╗
    ║   IP-in-IP Filtrator  (C++)             ║
    ║   Paketlarni qabul qilish va            ║
    ║   qoidalar asosida tekshirish           ║
    ╚══════════════════════════════════════════╝
)" << "\n";

    try {
        Filtrator filtrator(FILTRATOR_IP);  // "10.0.0.2"

        // ----- Namuna qoidalar -----

        // Blacklist: 192.168.1.100 ni bloklash
        filtrator.add_rule(FilterRule(
            FilterRule::DENY,
            "192.168.1.100", 0, 0,
            "Bloklangan IP: 192.168.1.100"
        ));

        // Faqat 8080 portga TCP ulanishga ruxsat
        filtrator.add_rule(FilterRule(
            FilterRule::ALLOW,
            "", 8080, PROTO_TCP,
            "TCP:8080 ga ruxsat"
        ));

        // Barcha UDP ni bloklash
        filtrator.add_rule(FilterRule(
            FilterRule::DENY,
            "", 0, PROTO_UDP,
            "UDP ni bloklash"
        ));

        // --------------------------------
        filtrator.start_listening();

    } catch (const std::exception &ex) {
        std::cerr << "[XATO] " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
