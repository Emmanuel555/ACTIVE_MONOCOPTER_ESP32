#include <Arduino.h>
//#include "comms.h"
#include "wifi_rec.h"
#include "optitrack_udp.h"

void setup(){
    send_ESPNOW_init();
}

void loop(){
    ESPNOW_loop();
    Serial.println("ESP32 loop running...");
    //comms_tester();
    //delay(4); // 250Hz max
}

    