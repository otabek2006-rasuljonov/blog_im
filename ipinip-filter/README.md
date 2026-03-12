# IP-in-IP Tarmoq Monitoring va Filtratsiya Tizimi (C++)

Bu loyiha **IP-in-IP (RFC 2003)** protokoli asosida qurilgan tarmoq monitoring va filtratsiya tizimini C++ da amalga oshiradi.

---

## 📐 Arxitektura

```
   [Mijoz / Client]
         |
         | TCP SYN / UDP (oddiy ulanish)
         v
  ┌─────────────────────┐
  │      SNIFFER        │   <- sniffer binary
  │  (10.0.0.1:eth0)    │
  │                     │
  │ 1. AF_PACKET socket │
  │    bilan paket ushla│
  │ 2. TCP SYN / UDP    │
  │    filtrla          │
  │ 3. IP-in-IP         │
  │    encapsulate      │
  └────────┬────────────┘
           │
           │  IP-in-IP tunnel (protokol 4)
           │
           │  ┌───────────────────────────┐
           │  │ Outer IP Header            │
           │  │  src  = 10.0.0.1 (sniffer)│
           │  │  dst  = 10.0.0.2 (filtrat)│
           │  │  proto= 4 (IPIP)          │
           │  ├───────────────────────────┤
           │  │ Inner IP Header            │
           │  │  src  = client_ip          │
           │  │  dst  = server_ip          │
           │  │  proto= 6/17 (TCP/UDP)    │
           │  ├───────────────────────────┤
           │  │ Mini Payload (4 bayt)      │
           │  │  client_port | server_port │
           │  └───────────────────────────┘
           │
           v
  ┌─────────────────────┐
  │     FILTRATOR       │   <- filtrator binary
  │  (10.0.0.2)         │
  │                     │
  │ 1. IPIP socket      │
  │    paket qabul qil  │
  │ 2. Outer header o'q │
  │ 3. Inner decapsulate│
  │ 4. Qoidalar tekshir │
  │    (first-match)    │
  │ 5. ALLOW / DENY     │
  └─────────────────────┘
```

---

## 📁 Loyiha Tuzilmasi

```
ipinip-filter/
├── CMakeLists.txt          # Build konfiguratsiyasi
├── README.md               # Shu fayl
└── src/
    ├── config.h            # IP manzillar, portlar, protokol konstantalari
    ├── models.h            # IPv4Header, TCPHeader, UDPHeader, ClientConnection, TunnelPacket
    ├── checksum.h          # RFC 1071 IP checksum hisoblash
    ├── tunnel.h / .cpp     # IP-in-IP encapsulator (raw socket, IP_HDRINCL)
    ├── sniffer.h / .cpp    # AF_PACKET raw sniffer (TCP SYN + UDP)
    ├── filtrator.h / .cpp  # IP-in-IP decapsulator + qoidalar filtri
    ├── sniffer_main.cpp    # Sniffer dasturi kirish nuqtasi
    └── filtrator_main.cpp  # Filtrator dasturi kirish nuqtasi
```

---

## 🔧 Modullar tavsifi

### `config.h`
Barcha konfiguratsiya konstantalari bir joyda:
- `LISTEN_INTERFACE` — tinglanadigan tarmoq interfeysi ("eth0")
- `LISTEN_PORT`      — filtrlash porti (8080)
- `SNIFFER_IP`       — sniffer server IP manzili
- `FILTRATOR_IP`     — filtrator server IP manzili
- `PROTO_IPIP/TCP/UDP` — protokol raqamlari

### `models.h`
Protokol header tuzilmalari (`#pragma pack(push,1)` bilan):
- `IPv4Header` — RFC 791, `version()`, `header_length()`, `src_str()` metodlari
- `TCPHeader`  — RFC 793, `is_syn()`, `is_ack()`, `is_rst()`, `is_fin()` metodlari
- `UDPHeader`  — RFC 768
- `ClientConnection` — ushlangan ulanish ma'lumotlari + `key()`, `to_string()`
- `TunnelPacket`     — tunnel metama'lumotlari + `to_string()`

### `checksum.h`
RFC 1071 Internet checksum funksiyasi:
```cpp
uint16_t compute_checksum(const void* data, std::size_t len);
```

### `tunnel.h / tunnel.cpp`
`IPinIPTunnel` sinfi:
- `SOCK_RAW + IPPROTO_RAW + IP_HDRINCL` orqali raw socket ochadi
- `build_ip_header()` — IPv4 header to'ldiradi va checksum hisoblaydi
- `encapsulate_and_send(conn)` — outer + inner IP header + mini payload yasab, filtratorga yuboradi

### `sniffer.h / sniffer.cpp`
`ConnectionSniffer` sinfi:
- `AF_PACKET + SOCK_RAW + ETH_P_IP` — barcha Ethernet freymlarini qabul qiladi
- `process_packet()` — Ethernet → IPv4 → TCP/UDP qatlamlarni ajratadi
- Faqat manzil port mos va TCP SYN (`flags=0x02, not ACK`) yoki UDP paketlarni qayta ishlaydi
- Takroriy ulanishlar `m_seen` hash-set orqali o'tkazib yuboriladi

### `filtrator.h / filtrator.cpp`
`Filtrator` sinfi:
- `SOCK_RAW + PROTO_IPIP` — IPIP protokol paketlarini qabul qiladi
- `process_ipip_packet()` — outer decapsulate → inner header o'qi → port ajrat → qoida qo'lla
- `add_rule(FilterRule)` — IP yoki prefiks asosida `allow/deny` qoidasi qo'shadi
- `apply_rules()` — first-match: birinchi mos qoida ishlatiladi; mos qoida yo'q bo'lsa — **default allow**

---

## 🏗️ Qurish (Build)

```bash
# Talab: cmake >= 3.16, g++ >= 11 (C++17)
cd ipinip-filter
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Natija: `build/sniffer` va `build/filtrator` ikkilik fayllar.

---

## 🚀 Ishga tushirish

### 1-Terminal — Filtrator (qabul qiluvchi server)
```bash
sudo ./build/filtrator 10.0.0.2
```

### 2-Terminal — Sniffer (tinglovchi)
```bash
sudo ./build/sniffer eth0 8080 10.0.0.1 10.0.0.2
```

### Argumentlar (ixtiyoriy)
```
sniffer    [interface] [port]  [local_ip]  [filtrator_ip]
filtrator  [listen_ip]
```
Argument berilmasa, `config.h` dagi standart qiymatlar ishlatiladi.

### Oddiy test
```bash
# Sniffer serverda (boshqa terminal):
curl http://<sniffer_server_ip>:8080/test

# Filtrator consolida ko'rinishi:
# [Filtrator] Qabul: TCP 203.0.113.50:52341 -> 10.0.0.1:8080
# [Filtrator] RUXSAT: 203.0.113.50
```

---

## ⚙️ Qoidalar misoli

`filtrator_main.cpp` da qoidalarni tartib bilan qo'shing (first-match):

```cpp
filtrator.add_rule({"192.168.1.100", false}); // Aniq IP bloklash
filtrator.add_rule({"10.",            true});  // 10.x.x.x — ruxsat
filtrator.add_rule({"172.16.",        false}); // 172.16.x.x — bloklash
// Qolgan barchaga default allow
```

---

## 📚 O'rganish manbalari

| Mavzu | Manba |
|-------|-------|
| IPv4 protokol | [RFC 791](https://tools.ietf.org/html/rfc791) |
| IP-in-IP | [RFC 2003](https://datatracker.ietf.org/doc/html/rfc2003) |
| Raw socketlar | [Beej's Guide](https://beej.us/guide/bgnet/html/) |
| Pcap asoslari | [tcpdump pcap(3)](https://www.tcpdump.org/manpages/pcap.3pcap.html) |
| C++ ma'lumotnoma | [cppreference.com](https://en.cppreference.com/w/) |

---

## ⚠️ Muhim eslatmalar

1. **Root huquqi** — raw socket ochish uchun `sudo` kerak.
2. **Linux only** — `AF_PACKET` va `linux/if_packet.h` Linux-ga xos.
3. **Interfeys nomi** — `ip link` buyrug'i bilan tarmoq interfeysi nomini aniqlang.
4. **Xavfsizlik** — Bu dastur faqat ta'lim va test maqsadida yaratilgan. Ishlab chiqarish muhitida qo'shimcha xavfsizlik choralarini ko'ring.
