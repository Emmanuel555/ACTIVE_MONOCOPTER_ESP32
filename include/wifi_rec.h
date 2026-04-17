#pragma once // this prevents multiple inclusions
#include <esp_now.h>

void connectToWiFi();
//void connectToServer();
void start_wifi_serial();
void wifi_recursion();
void sendUDPMessage(const String& msg);
void motor_wifi_recursion();
void dshot_motor_wifi_recursion();
void onReceive(const uint8_t *mac, const uint8_t *data, int len);
void ESPNOW_init();
void start_ESPNOW_serial();
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void send_ESPNOW_init();
void ESPNOW_loop();