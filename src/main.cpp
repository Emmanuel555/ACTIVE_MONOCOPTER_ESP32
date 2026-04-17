#include <Arduino.h>
//#include "comms.h"
#include "wifi_rec.h"
#include "optitrack_udp.h"

void setup(){
    send_ESPNOW_init();
}

void loop(){
    ESPNOW_loop();
    delay(4); // 250Hz max
}

    