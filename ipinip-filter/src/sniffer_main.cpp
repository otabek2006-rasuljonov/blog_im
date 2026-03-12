/**
 * sniffer_main.cpp - Sniffer dasturi kirish nuqtasi
 *
 * Ishlatish (root huquqi kerak):
 *   sudo ./sniffer [interface] [port] [sniffer_ip] [filtrator_ip]
 *
 * Misol:
 *   sudo ./sniffer eth0 8080 10.0.0.1 10.0.0.2
 */

#include "sniffer.h"
#include "config.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// SIGINT (Ctrl+C) uchun global bayroq
static volatile sig_atomic_t g_stop = 0;

static void signal_handler(int /*sig*/) {
    g_stop = 1;
}

int main(int argc, char* argv[]) {
    // Argument tahlili
    std::string interface  = (argc > 1) ? argv[1] : Config::LISTEN_INTERFACE;
    uint16_t    port       = (argc > 2) ? static_cast<uint16_t>(std::atoi(argv[2]))
                                        : Config::LISTEN_PORT;
    std::string local_ip   = (argc > 3) ? argv[3] : Config::SNIFFER_IP;
    std::string remote_ip  = (argc > 4) ? argv[4] : Config::FILTRATOR_IP;

    std::cout << R"(
  ╔══════════════════════════════════════════╗
  ║   IP-in-IP Connection Sniffer (C++)     ║
  ║   Aloqani tinglovchi va tunnel          ║
  ╚══════════════════════════════════════════╝
)" << "\n";

    std::cout << "  Interfeys : " << interface  << "\n"
              << "  Port      : " << port       << "\n"
              << "  Lokal IP  : " << local_ip   << "\n"
              << "  Filtrator : " << remote_ip  << "\n\n";

    // Signal handler o'rnatish
    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        ConnectionSniffer sniffer(interface, port, local_ip, remote_ip);
        sniffer.start();
    } catch (const std::exception& ex) {
        std::cerr << "[Xato] " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
