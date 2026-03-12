#pragma once

/**
 * checksum.h - IP header checksum hisoblash (RFC 1071)
 *
 * Internet checksum algoritmi:
 *   1. Barcha 16-bitli so'zlarni yig'amiz.
 *   2. Yig'indining 16 bitdan yuqori qismini (carry) pastki qismga qo'shamiz.
 *   3. Natijaning bitlarini inversiyalaymiz (one's complement).
 */

#include <cstdint>
#include <cstddef>

/**
 * @brief Berilgan ma'lumotlar uchun Internet checksum hisoblaydi (RFC 1071).
 *
 * @param data  Checksum hisoblanadigan ma'lumotlar ko'rsatkichi
 * @param len   Ma'lumotlar uzunligi (baytlarda)
 * @return      16-bitli checksum (host byte order)
 */
inline uint16_t compute_checksum(const void* data, std::size_t len) {
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    uint32_t sum = 0;

    // 16-bitli so'zlar bo'yicha yig'amiz
    while (len > 1) {
        uint16_t word = static_cast<uint16_t>((ptr[0] << 8) | ptr[1]);
        sum += word;
        ptr += 2;
        len -= 2;
    }

    // Toq bayt qolsa, uni 0 bilan to'ldirish
    if (len == 1) {
        sum += static_cast<uint16_t>(ptr[0] << 8);
    }

    // Carry bitlarni qo'shamiz (fold 32-bit into 16-bit)
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // One's complement
    return static_cast<uint16_t>(~sum & 0xFFFF);
}
