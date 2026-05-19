#pragma once

void snail_ESPNOW_init();
void start_snailESPNOW_serial();
void snail_onReceive(const uint8_t *mac, const uint8_t *data, int len);