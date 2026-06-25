#pragma once
#include <stdint.h>
struct state_estimation {
    float x, y;    // position (mm)
    float vx, vy;  // velocity (mm/s)
    float mag;     // heading (deg)
};

extern state_estimation states;

struct actuator_cmd5 {
    float values[5];
};

void snail_ESPNOW_init(String mode);
void start_snailESPNOW_serial(String mode);
void snail_onReceive(const uint8_t *mac, const uint8_t *data, int len);
void send_states(state_estimation states);

// standalone, not wired to esp_now_register_recv_cb (only one recv callback
// allowed) - call this manually wherever the 5-float packet actually arrives
void snail_onReceive_5f(const uint8_t *mac, const uint8_t *data, int len);
void send_to_teensy_5f(actuator_cmd5 cmd);