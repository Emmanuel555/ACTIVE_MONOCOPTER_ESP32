#include <Arduino.h>
#include "snail_rx_comms.h"
#include "snail_tx_comms.h"
#include <WiFi.h>

void setup(){
    // for receiving
    //start_snailESPNOW_serial();
    
    // for sending
    send_ESPNOW_init();
}

void loop(){
    // for sending
    ESPNOW_loop();
    Serial.println("ESP32 loop running...");
    //Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
    //comms_tester();
    //delay(4); // 250Hz max
}

    