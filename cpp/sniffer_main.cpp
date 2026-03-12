/**
 * sniffer_main.cpp — Sniffer dasturi kirish nuqtasi
 *
 * Ishga tushirish:
 *   sudo ./sniffer
 */

#include "sniffer.h"
#include "config.h"
#include <iostream>

int main()
{
    std::cout << R"(
    ╔══════════════════════════════════════════╗
    ║   IP-in-IP Connection Sniffer  (C++)    ║
    ║   Aloqani tinglovchi va tunnel          ║
    ╚══════════════════════════════════════════╝
)" << "\n";

    try {
        ConnectionSniffer sniffer(
            LISTEN_INTERFACE,   // "eth0"
            LISTEN_PORT,        // 8080
            SNIFFER_IP,         // "10.0.0.1"
            FILTRATOR_IP        // "10.0.0.2"
        );
        sniffer.start();
    } catch (const std::exception &ex) {
        std::cerr << "[XATO] " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
