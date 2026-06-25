#include <WiFi.h>
#include <WiFiUdp.h>
#include "snail_tx_comms.h"
#include <esp_now.h>


// ESP32 UART pins connected to Teensy
// change these to match your wiring
constexpr int RXD2 = 44;
constexpr int TXD2 = 43;


// WIN ESP32 MAC address - get this from WIN ESP32's Serial monitor
uint8_t SnailMacAddress[6];
esp_now_peer_info_t peerInfo;
int inputs = 4; // number of input control values expected in each packet


void setSnailMacAddress(uint8_t *mac) {
    memcpy(SnailMacAddress, mac, 6);
}


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


void control_loop() {
    /* if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        Serial.println("Reading from PC Serial: " + msg);
        msg.trim();
        esp_now_send(pcbMacAddress, (uint8_t*)msg.c_str(), msg.length());
    } */

    if (Serial.available() >= inputs * 4) {
        uint8_t data[inputs * 4];
        Serial.readBytes(data, inputs * 4);
        esp_now_send(SnailMacAddress, data, inputs * 4);

        Serial.print("Sent from com control packet: ");
        for (int i = 0; i < inputs; i++) {
            float value;
            memcpy(&value, data + i * 4, 4);
            Serial.printf(" %.3f", value);
        }
        Serial.println();
    }
}


void comms_tester() {
  String testMsg = "Computer sending paused - ESP32 alive";
  esp_now_send(SnailMacAddress, (uint8_t*)testMsg.c_str(), testMsg.length());
}


void teensy_start(){
  Serial.println("Connection to teensy binding...");  // USB debug
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);  // UART to Teensy

  // warmup Serial1 to Teensy - put this last
  unsigned long start = millis();
  while (millis() - start < 1000) {
      Serial1.println(1000);
  }

  Serial.println("Teensy connected!");
}


void feedback_states(float x, float y, float vx, float vy, float mag) {
    uint8_t buf[20];
    memcpy(buf,      &x,   4);
    memcpy(buf + 4,  &y,   4);
    memcpy(buf + 8,  &vx,  4);
    memcpy(buf + 12, &vy,  4);
    memcpy(buf + 16, &mag, 4);
    //Serial.printf("SnailMacAddress: %02X:%02X:%02X:%02X:%02X:%02X\n", SnailMacAddress[0], SnailMacAddress[1], SnailMacAddress[2], SnailMacAddress[3], SnailMacAddress[4], SnailMacAddress[5]);
    esp_err_t sendResult = esp_now_send(SnailMacAddress, buf, sizeof(buf));
    Serial.printf("esp_now_send result: %s, WiFi channel: %d\n", sendResult == ESP_OK ? "OK" : "FAILED", WiFi.channel());
}
























