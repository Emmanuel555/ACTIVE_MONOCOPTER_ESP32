#pragma once
#include <stdint.h>
struct state_estimation {
    float x, y;    // position (mm)
    float vx, vy;  // velocity (mm/s)
    float mag;     // heading (deg)
};

extern state_estimation states;

void snail_ESPNOW_init();
void start_snailESPNOW_serial();
void snail_onReceive(const uint8_t *mac, const uint8_t *data, int len);
void send_states(state_estimation states);