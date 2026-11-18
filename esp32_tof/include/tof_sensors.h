#pragma once

#include <Arduino.h>

// ---------------------------------------------------------------------------
// VL6180X ToF sensor settings - Seeed XIAO ESP32-S3
// ---------------------------------------------------------------------------

constexpr uint8_t TOF_NUM_SENSORS = 5;

// SHUT (CE) pin per sensor, XIAO silkscreen names.
// D2 skipped (strapping pin), D6/D7 kept free for UART to the Teensy.
const uint8_t TOF_CE_PINS[TOF_NUM_SENSORS] = {D0, D1, D2, D8, D9};

// I2C pins (shared SDA/SCL rails)
const uint8_t TOF_SDA_PIN = D4;
const uint8_t TOF_SCL_PIN = D5;

constexpr uint8_t  TOF_DEFAULT_ADDR = 0x29;    // every sensor at power-up
constexpr uint8_t  TOF_BASE_ADDR    = 0x30;    // sensor i -> 0x30 + i
constexpr uint32_t TOF_I2C_CLOCK_HZ = 400000;  // drop to 100000 if unreliable
constexpr uint16_t TOF_TIMEOUT_MS   = 100;
constexpr int16_t  TOF_INVALID      = -1;
constexpr uint8_t  TOF_SYNC_BYTE    = 0xBB;

// Packet: [0xBB][uint32 time_ms][int16 s0..s4 mm], little-endian
constexpr int TOF_PACKET_SIZE = 1 + 4 + 2 * TOF_NUM_SENSORS;  // 15 bytes

// Per-sensor calibration: corrected_mm = raw_mm * scale + offset
// Copied from the Teensy version (same modules, same breadboard).
//                                          s0    s1     s2     s3    s4
const float TOF_SCALE[TOF_NUM_SENSORS]     = {1.0f, 1.0f,  1.0f,  1.0f, 1.0f};
const float TOF_OFFSET_MM[TOF_NUM_SENSORS] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
// Neutral values, for recalibrating from scratch:
// const float TOF_SCALE[TOF_NUM_SENSORS]     = {1, 1, 1, 1, 1};
// const float TOF_OFFSET_MM[TOF_NUM_SENSORS] = {0, 0, 0, 0, 0};

// ---------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------

uint8_t initSensors();              // returns number of sensors that are OK
int16_t readSensor(uint8_t i);      // corrected mm, or TOF_INVALID (-1)
void printHeader();                 // time_ms,s0_mm,s1_mm,s2_mm,s3_mm,s4_mm
void printSweep();                  // CSV: time_ms,s0,s1,s2,s3,s4
void printReadings();               // t=1523 ms  s0: 87  s1: 92 ...
void buildPacket(uint8_t out[TOF_PACKET_SIZE]);  // one sweep -> 15-byte packet
void sendReadingsSerial1();         // buildPacket() + Serial1.write()
