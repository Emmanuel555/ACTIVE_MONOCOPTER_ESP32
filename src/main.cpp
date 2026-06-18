#include <Arduino.h>
#include "snail_rx_comms.h"
#include "snail_tx_comms.h"
#include "servo4.h"
#include <WiFi.h>

void setup(){
    // for receiving
    start_snailESPNOW_serial(); //

    // for sending
    send_ESPNOW_init_lite(); //

    servo4_init();
}

void loop(){
    // for sending
    ESPNOW_loop(); //

    static unsigned long lastToggle = 0;
    static int angle = 60;
    if (millis() - lastToggle >= 1000) {
        angle = (angle == 60) ? 120 : 60;
        lastToggle = millis();
    }
    servo4_writeAngles(angle, angle);   // pins 1, 2 sweep 60 <-> 120

    //servo4_writeContinuous(1500, 1500); // pins 6, 43 stopped
    //Serial.println("ESP32 loop running..."); //
    //Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
    //comms_tester();
    //delay(4); // 250Hz max
}

    