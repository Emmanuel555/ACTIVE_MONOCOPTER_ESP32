#pragma once
#include <esp_now.h>

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void send_ESPNOW_init();
void ESPNOW_loop();
void comms_tester();
void send_3pwm(int pwm1, int pwm2, int pwm3);
void send_ESPNOW_init_lite(); // init without warmup for faster startup