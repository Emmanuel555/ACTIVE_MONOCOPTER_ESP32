#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// MAC addresses for the ToF ESP-NOW link.
// Lives in esp32_tof/include/ (on the xiao_tof env's include path).
// Another env (e.g. the receiving ESP32 on the computer) needs
// -I esp32_tof/include in its build_flags to use it.
// ---------------------------------------------------------------------------

// ESP32 that reads the 5 VL6180X ToF sensors (sender)
// 1C:DB:D4:5A:7D:BC
const uint8_t TOF_ESP32_MAC[6] = {0x1C, 0xDB, 0xD4, 0x5A, 0x7D, 0xBC};
