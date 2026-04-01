#include <Arduino.h>
//#include "comms.h"
#include "wifi_rec.h"

void setup(){
    start_wifi_serial();
    //Serial.begin(115200);
    //delay(3000);
    //Serial.println("BOOT OK");
}

void loop(){
    wifi_recursion();
    //Serial.println("RUNNING");
    //delay(500);
}

    