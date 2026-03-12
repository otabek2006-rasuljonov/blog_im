/**
 * filtrator_main.cpp - Filtrator dasturi kirish nuqtasi
 *
 * Ishlatish (root huquqi kerak):
 *   sudo ./filtrator [filtrator_ip]
 *
 * Misol:
 *   sudo ./filtrator 10.0.0.2
 *
 * Qoidalarni dastur ichida yoki konfiguratsiya faylidan yuklash mumkin.
 * Hozirgi misolda bir nechta demo qoida qo'shilgan.
 */

#include "filtrator.h"
#include "config.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

static volatile sig_atomic_t g_stop = 0;

static void signal_handler(int /*sig*/) {
    g_stop = 1;
}

int main(int argc, char* argv[]) {
    std::string listen_ip = (argc > 1) ? argv[1] : Config::FILTRATOR_IP;

    std::cout << R"(
  ╔══════════════════════════════════════════╗
  ║   IP-in-IP Filtrator (C++)              ║
  ║   Paketlarni qabul qilish va filtrash   ║
  ╚══════════════════════════════════════════╝
)" << "\n";

    std::cout << "  Tinglash IP: " << listen_ip << "\n\n";

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        Filtrator filtrator(listen_ip);

        // ---- Demo filtrlash qoidalari ----
        // 1. 192.168.1.100 ni bloklash
        filtrator.add_rule({"192.168.1.100", false});

        // 2. Butun 10.0.0.0/8 tarmog'iga ruxsat
        filtrator.add_rule({"10.", true});

        // 3. 172.16.0.0/12 tarmog'ini bloklash
        filtrator.add_rule({"172.16.", false});

        // Qolganlarga standart ruxsat (Filtrator::apply_rules() da default allow)

        filtrator.start();
    } catch (const std::exception& ex) {
        std::cerr << "[Xato] " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
