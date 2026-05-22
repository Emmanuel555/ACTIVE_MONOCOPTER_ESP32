#include <WiFi.h>
#include <WiFiUdp.h>
#include "snail_tx_comms.h"
#include <esp_now.h>


// WIN ESP32 MAC address - get this from WIN ESP32's Serial monitor
uint8_t SnailMacAddress[] = {0xD8, 0x3B, 0xDA, 0x45, 0x88, 0x18};

esp_now_peer_info_t peerInfo;

int inputs = 4; // number of PWM values expected in each packet

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
Serial.printf("Send status: %s\n", status == ESP_NOW_SEND_SUCCESS ? "SENT (RX Online)" : "FAILED (RX Offline)");
}


void send_ESPNOW_init() {
    Serial.begin(115200);
    setCpuFrequencyMhz(80);          // reduce CPU speed
    //WiFi.setTxPower(WIFI_POWER_2dBm); // reduce TX power
    WiFi.mode(WIFI_STA);
    esp_now_init();
    //esp_now_register_send_cb(onSent); // too much overhead for this callback, it causes significant latency spikes, so we will just check status in loop instead

    memcpy(peerInfo.peer_addr, SnailMacAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Peer added successfully");

    // warmup - send dummy packets to stabilize ESP-NOW
    Serial.println("Warming up ESP-NOW...");
    unsigned long start = millis();
    while (millis() - start < 2000) {
        uint8_t dummy = 0;
        esp_now_send(SnailMacAddress, &dummy, 1);
        //delay(4); // 250Hz
    }
    Serial.println("Ready!");
}


void send_ESPNOW_init_lite() {
    setCpuFrequencyMhz(80);          // reduce CPU speed
    memcpy(peerInfo.peer_addr, SnailMacAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Peer added successfully");

    // warmup - send dummy packets to stabilize ESP-NOW
    //Serial.println("Warming up ESP-NOW...");
    unsigned long start = millis();
    while (millis() - start < 2000) {
        uint8_t dummy = 0;
        esp_now_send(SnailMacAddress, &dummy, 1);
        //delay(4); // 250Hz
    }
    Serial.println("Ready to send as well!");
}



void ESPNOW_loop() {
    /* if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        Serial.println("Reading from PC Serial: " + msg);
        msg.trim();
        esp_now_send(pcbMacAddress, (uint8_t*)msg.c_str(), msg.length());
    } */

    if (Serial.available() >= inputs * 2) {
        uint8_t data[inputs * 2];
        Serial.readBytes(data, inputs * 2);
        esp_now_send(SnailMacAddress, data, inputs * 2);
    }
}


void comms_tester() {
  String testMsg = "Computer sending paused - ESP32 alive";
  esp_now_send(SnailMacAddress, (uint8_t*)testMsg.c_str(), testMsg.length());
}


void send_3pwm(int pwm1, int pwm2, int pwm3) {
    uint8_t buf[6];
    buf[0] = pwm1 & 0xFF;
    buf[1] = (pwm1 >> 8) & 0xFF;
    buf[2] = pwm2 & 0xFF;
    buf[3] = (pwm2 >> 8) & 0xFF;
    buf[4] = pwm3 & 0xFF;
    buf[5] = (pwm3 >> 8) & 0xFF;
    esp_now_send(SnailMacAddress, buf, 6);
}
























