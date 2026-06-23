#pragma once
#include <esp_now.h>

extern uint8_t SnailMacAddress[6];

void setSnailMacAddress(uint8_t *mac);
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void send_ESPNOW_init();
void control_loop();
void comms_tester();
void feedback_states(float x, float y, float vx, float vy, float mag);
void send_ESPNOW_init_lite(); // init without warmup for faster startup
void teensy_start();