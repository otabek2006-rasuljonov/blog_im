# 📚 C++ uchun IP-in-IP Tarmoq Monitoringi va Filtratsiyasi
## Yakuniy O'rganish Rejasi (Learning Plan)

---

## 🏗️ Loyiha Arxitekturasi

```
   [Mijoz / Client]
          │
          │  TCP SYN / UDP packet
          ▼
┌─────────────────────────┐
│    SNIFFER (sniffer)    │  ← tarmoq paketlarini ushlaydi
│    IP: 10.0.0.1         │    (SOCK_RAW, root/CAP_NET_RAW)
│                         │
│  1. Paketni ushlash     │
│  2. Client IP ni olish  │
│  3. IP-in-IP package    │
└──────────┬──────────────┘
           │
           │  IP-in-IP Encapsulated Packet
           │  ┌─────────────────────────────┐
           │  │ Outer IP: 10.0.0.1→10.0.0.2│
           │  │ Protocol: 4 (IPIP)          │
           │  │  ┌──────────────────────┐   │
           │  │  │Inner IP: client→srv  │   │
           │  │  │Payload: src/dst port │   │
           │  │  └──────────────────────┘   │
           │  └─────────────────────────────┘
           ▼
┌──────────────────────────┐
│   FILTRATOR (filtrator)  │  ← paketlarni qabul qiladi va tekshiradi
│   IP: 10.0.0.2           │    (SOCK_RAW, PROTO_IPIP=4)
│                          │
│  1. Decapsulate          │
│  2. Client IP ajratish   │
│  3. Qoidalar tekshirish  │
│  4. ALLOW / DENY         │
└──────────────────────────┘
```

---

## 📁 Loyiha Fayllar Tuzilmasi

```
cpp/
├── config.h            ← Konfiguratsiya konstantalari
├── models.h            ← ClientConnection, TunnelPacket (data models)
├── tunnel.h            ← IPinIPTunnel interfeysi
├── tunnel.cpp          ← IP-in-IP encapsulation, raw socket, checksum
├── sniffer.h           ← ConnectionSniffer interfeysi
├── sniffer.cpp         ← SOCK_RAW paket ushlash, TCP SYN/UDP aniqlash
├── filtrator.h         ← Filtrator + FilterRule interfeysi
├── filtrator.cpp       ← IPIP decapsulation, qoidalar tizimi
├── sniffer_main.cpp    ← Sniffer dasturi kirish nuqtasi
├── filtrator_main.cpp  ← Filtrator dasturi kirish nuqtasi
└── CMakeLists.txt      ← CMake build konfiguratsiyasi
```

---

## 🗺️ O'rganish Yo'l Xaritasi (Roadmap)

### 📗 1-Daraja: Asoslar (1–2 hafta)

| # | Mavzu | Resurslar |
|---|-------|-----------|
| 1 | **C++ asoslari** — pointer, struct, class | [cppreference.com](https://cppreference.com) |
| 2 | **TCP/IP modeli** — OSI qatlamlari, IP/TCP/UDP sarlavhalari | RFC 791 (IP), RFC 793 (TCP) |
| 3 | **Linux socket API** — `socket()`, `bind()`, `sendto()`, `recvfrom()` | `man 2 socket`, `man 7 ip` |
| 4 | **Ikkilik ma'lumotlar** — `struct`, `htons/ntohs`, `inet_aton/inet_ntoa` | `man 3 byteorder` |

#### 🔬 Amaliy mashqlar:
- [ ] Oddiy TCP echo server/client yozish
- [ ] UDP socket bilan ishlash
- [ ] `struct iphdr`, `struct tcphdr` qo'lda to'ldirish va chiqarish

---

### 📘 2-Daraja: Raw Socket va IP Sarlavhalar (2–3 hafta)

| # | Mavzu | Resurslar |
|---|-------|-----------|
| 1 | **Raw Socket** — `SOCK_RAW`, `IPPROTO_RAW`, `IP_HDRINCL` | `man 7 raw` |
| 2 | **IP Header formati** — RFC 791, checksum hisoblash | [RFC 791](https://tools.ietf.org/html/rfc791) |
| 3 | **TCP Header formati** — flags (SYN, ACK, FIN), sequence | [RFC 793](https://tools.ietf.org/html/rfc793) |
| 4 | **Packet Sniffer** — `recvfrom()` bilan paket ushlash | [Beej's Guide](https://beej.us/guide/bgnet/) |
| 5 | **Checksum** — RFC 1071, one's complement | [RFC 1071](https://tools.ietf.org/html/rfc1071) |

#### 🔬 Amaliy mashqlar:
- [ ] Raw socket bilan barcha IP paketlarni ekranga chiqarish
- [ ] Faqat TCP SYN paketlarni filtrlash
- [ ] `struct iphdr` yordamida paket tarkibini tahlil qilish

---

### 📙 3-Daraja: IP-in-IP Tunnel (2–3 hafta)

| # | Mavzu | Resurslar |
|---|-------|-----------|
| 1 | **IP-in-IP RFC 2003** — encapsulation/decapsulation | [RFC 2003](https://tools.ietf.org/html/rfc2003) |
| 2 | **Paket yaratish** — inner + outer IP header | `tunnel.cpp` fayli |
| 3 | **`sendto()` bilan raw paket yuborish** | `man 2 sendto` |
| 4 | **Linux TUN/TAP interfeysi** (ixtiyoriy, yuqori daraja) | `man 4 tun` |

#### 🔬 Amaliy mashqlar:
- [ ] `tunnel.cpp` kodini o'rganib, debug loglar qo'shish
- [ ] Paketni `tcpdump` yordamida kuzatish
- [ ] Wireshark bilan IP-in-IP paketni ko'rish

---

### 📕 4-Daraja: Monitoring Tizimi Qurish (3–4 hafta)

| # | Mavzu | Resurslar |
|---|-------|-----------|
| 1 | **`sniffer.cpp`** — ConnectionSniffer sinfi | Ushbu loyiha |
| 2 | **Signal handling** — `SIGINT`, `SIGTERM` | `man 2 signal` |
| 3 | **Thread safety** — `std::mutex`, `std::unordered_set` | cppreference |
| 4 | **`filtrator.cpp`** — FilterRule tizimi | Ushbu loyiha |
| 5 | **CMake** — loyiha qurishni avtomatlashtirish | [cmake.org/cmake/help](https://cmake.org/cmake/help) |

#### 🔬 Amaliy mashqlar:
- [ ] Loyihani build qilish: `cmake -B build && cmake --build build`
- [ ] Sniffer va filtratoni alohida terminallarda ishga tushirish
- [ ] `nc` yordamida sinov ulanish yuborish va loglarda ko'rish
- [ ] Yangi filtr qoidalari qo'shish (masalan, ma'lum IP ni bloklash)

---

### 🚀 5-Daraja: Kengaytirish (4+ hafta)

| Xususiyat | Tavsif |
|-----------|--------|
| **Rate Limiting** | Bir IP dan daqiqada n ta ulanishdan ko'pini bloklash |
| **Geo-filtering** | MaxMind GeoIP2 bilan mamlakatga ko'ra filtrlash |
| **Logging** | `spdlog` yoki `log4cpp` bilan structured logging |
| **Config fayl** | YAML/JSON qoidalar fayli (yamlcpp yoki nlohmann/json) |
| **REST API** | `cpp-httplib` bilan qoidalarni HTTP orqali boshqarish |
| **TUN interfeys** | Kernel darajasida tunnel (tun.ko moduli) |
| **iptables integratsiya** | `libnetfilter_queue` bilan kernel filter |

---

## 🛠️ Qurishni Boshlash

### Talablar
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake

# CentOS/RHEL
sudo yum install gcc-c++ cmake
```

### Build
```bash
cd cpp
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Ishga tushirish (ikkita terminal kerak)

**Terminal 1 — Filtrator (10.0.0.2 da):**
```bash
sudo ./build/filtrator
```

**Terminal 2 — Sniffer (10.0.0.1 da):**
```bash
sudo ./build/sniffer
```

**Terminal 3 — Sinov:**
```bash
# TCP ulanish sinovi
nc -v 10.0.0.1 8080

# UDP sinovi
echo "test" | nc -u 10.0.0.1 8080
```

### Paketlarni kuzatish (Wireshark / tcpdump)
```bash
# IP-in-IP paketlarni filtrlash
sudo tcpdump -i eth0 'proto 4' -v

# Wireshark filtri
ip.proto == 4
```

---

## 📖 Asosiy Resurslar

### Kitoblar
| Kitob | Muallif | Mavzu |
|-------|---------|-------|
| *TCP/IP Illustrated, Vol. 1* | W. Richard Stevens | TCP/IP asoslari |
| *Unix Network Programming, Vol. 1* | W. Richard Stevens | Socket dasturlash |
| *The Linux Programming Interface* | Michael Kerrisk | Linux system calls |
| *Computer Networks* | Andrew S. Tanenbaum | Tarmoq nazariyasi |

### RFC Standartlar
| RFC | Mavzu |
|-----|-------|
| [RFC 791](https://tools.ietf.org/html/rfc791) | Internet Protocol (IPv4) |
| [RFC 793](https://tools.ietf.org/html/rfc793) | Transmission Control Protocol |
| [RFC 768](https://tools.ietf.org/html/rfc768) | User Datagram Protocol |
| [RFC 2003](https://tools.ietf.org/html/rfc2003) | IP Encapsulation within IP (IP-in-IP) |
| [RFC 1071](https://tools.ietf.org/html/rfc1071) | Checksum hisoblash |

### Online Resurslar
| Resurs | Tavsif |
|--------|--------|
| [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) | Socket dasturlash bo'yicha eng yaxshi qo'llanma |
| [cppreference.com](https://en.cppreference.com) | C++ standart kutubxona |
| [Linux man pages](https://man7.org/linux/man-pages/) | System call va API hujjatlari |
| [Wireshark Protocol Reference](https://wiki.wireshark.org/Protocols) | Protokol tahlili |

### GitHub Loyihalar
| Loyiha | Tavsif |
|--------|--------|
| [google/gopacket](https://github.com/google/gopacket) | Packet processing kutubxonasi |
| [the-tcpdump-group/libpcap](https://github.com/the-tcpdump-group/libpcap) | Paket ushlash kutubxonasi |
| [secdev/scapy](https://github.com/secdev/scapy) | Python packet manipulation |

---

## ✅ O'rganish Tekshiruvi (Checklist)

### Nazariy bilimlar
- [ ] IP header formatini (`struct iphdr`) tushunaman
- [ ] TCP SYN, ACK, FIN flaglarini tushunaman
- [ ] UDP va TCP farqini tushunaman
- [ ] IP-in-IP encapsulation/decapsulation jarayonini tushunaman
- [ ] RFC 1071 checksum algoritmini tushunaman

### Amaliy ko'nikmalar
- [ ] Raw socket yaratataman (`SOCK_RAW`)
- [ ] IP header qurataman (`build_ip_header`)
- [ ] TCP SYN paketlarni filtrlayolaman
- [ ] IP-in-IP paket yuborataman (`encapsulate_and_send`)
- [ ] IP-in-IP paketni qabul qilib, inner IP ajratib olaman
- [ ] Filtr qoidalarini qo'sha olaman

### Loyiha
- [ ] CMake bilan loyihani build qilyapman
- [ ] Sniffer va filtratoni bir vaqtda ishga tushira olaman
- [ ] `tcpdump`/Wireshark bilan IP-in-IP paketlarni ko'ra olaman
- [ ] Yangi filtr qoidalari yoza olaman

---

## 📊 O'rganish Jadvali (8 hafta)

| Hafta | Mavzu | Soat |
|-------|-------|------|
| 1 | C++ asoslari (pointer, struct, class, STL) | 10 soat |
| 2 | TCP/IP nazariyasi, RFC 791/793/768 o'qish | 8 soat |
| 3 | Linux socket API, oddiy TCP/UDP misollar | 10 soat |
| 4 | Raw socket, paket ushlash, IP header tahlil | 12 soat |
| 5 | IP-in-IP RFC 2003, tunnel.cpp o'rganish | 10 soat |
| 6 | Sniffer yaratish, TCP SYN aniqlash | 12 soat |
| 7 | Filtrator va qoidalar tizimi | 10 soat |
| 8 | Sinov, debug, kengaytirish | 8 soat |

**Jami: ~80 soat**

---

## ⚠️ Muhim Eslatmalar

1. **Root huquqi** — Barcha dasturlar `sudo` bilan ishga tushirilishi kerak
   (yoki `CAP_NET_RAW` capability berilishi kerak)

2. **Faqat Linux** — Bu kod Linux uchun yozilgan. macOS va Windows da
   farqli API lar ishlatiladi

3. **Sinov muhiti** — Haqiqiy tarmoqda sinov qilishdan oldin virtual
   mashinada (VirtualBox/VMware/QEMU) sinab ko'ring

4. **`tcpdump` va Wireshark** — Debug qilishning eng yaxshi usuli

```bash
# IP-in-IP paketlarni real-time ko'rish
sudo tcpdump -i lo 'proto 4' -X -v
```
