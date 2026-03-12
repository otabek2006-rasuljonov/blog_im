#pragma once

/**
 * config.h - IP-in-IP Filter System konfiguratsiyasi
 *
 * Bu faylda sniffer va filtrator uchun barcha sozlamalar mavjud.
 * Ishlatishdan oldin o'z tarmoq sozlamalaringizga mos ravishda
 * quyidagi konstantalarni o'zgartiring.
 */

#include <cstdint>

namespace Config {

// ----------------------------------------------------------------
// Sniffer sozlamalari
// ----------------------------------------------------------------

/// Tinglanadigan tarmoq interfeysi nomi (masalan: "eth0", "ens3")
constexpr const char* LISTEN_INTERFACE = "eth0";

/// Filtrlash uchun tinglanadigan TCP/UDP port
constexpr uint16_t LISTEN_PORT = 8080;

// ----------------------------------------------------------------
// IP manzillar
// ----------------------------------------------------------------

/// Sniffer (encapsulator) serveri IP manzili
constexpr const char* SNIFFER_IP = "10.0.0.1";

/// Filtrator (decapsulator) serveri IP manzili
constexpr const char* FILTRATOR_IP = "10.0.0.2";

// ----------------------------------------------------------------
// Protokol konstantalari
// ----------------------------------------------------------------

/// IP-in-IP protokol raqami (RFC 2003)
constexpr uint8_t PROTO_IPIP = 4;

/// TCP protokol raqami
constexpr uint8_t PROTO_TCP = 6;

/// UDP protokol raqami
constexpr uint8_t PROTO_UDP = 17;

/// ICMP protokol raqami
constexpr uint8_t PROTO_ICMP = 1;

// ----------------------------------------------------------------
// Filtrator sozlamalari
// ----------------------------------------------------------------

/// Paket qabul qilish uchun maksimal bufer hajmi (baytlarda)
constexpr int RECV_BUFFER_SIZE = 65535;

/// Bir soniyada bir IP dan ruxsat etilgan maksimal ulanish soni (rate limit)
constexpr int RATE_LIMIT_PER_SECOND = 100;

} // namespace Config
