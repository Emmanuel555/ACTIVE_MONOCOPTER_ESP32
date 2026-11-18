#include <Arduino.h>
#include <WiFi.h>
#include "tof_sensors.h"

void setup() {
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && millis() - start < 3000) {
        delay(10);
    }

    // print this board's MAC address first (WiFi must be in STA mode to read it)
    WiFi.mode(WIFI_STA);
    Serial.printf("ESP32 MAC: %s\n", WiFi.macAddress().c_str());

    initSensors();
}

void loop() {
    printReadings();
    //Serial.printf("ESP32 MAC: %s\n", WiFi.macAddress().c_str());
    delay(20);
}


// 1C:DB:D4:5A:7D:BC