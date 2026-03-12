#pragma once

/**
 * IP-in-IP Filter System - Konfiguratsiya
 * Configuration constants for the IP-in-IP network monitoring system.
 */

// Sniffer sozlamalari (Sniffer settings)
#define LISTEN_INTERFACE    "eth0"      // Tinglanadigan tarmoq interfeysi
#define LISTEN_PORT         8080        // Tinglanadigan port (TCP)

// IP-in-IP Tunnel sozlamalari (Tunnel settings)
#define SNIFFER_IP          "10.0.0.1"  // Sniffer (encapsulator) IP
#define FILTRATOR_IP        "10.0.0.2"  // Filtrator (decapsulator) IP

// IP protokol raqamlari (IP protocol numbers)
#define PROTO_IPIP          4           // IP-in-IP (RFC 2003)
#define PROTO_TCP           6
#define PROTO_UDP           17

// Filtrator sozlamalari
#define FILTRATOR_LISTEN_PORT  9090
#define MAX_PACKET_SIZE        65535
#define IP_HEADER_SIZE         20
