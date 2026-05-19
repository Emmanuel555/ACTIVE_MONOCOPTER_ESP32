#include <Arduino.h>
#include "snail_rx_comms.h"
#include "snail_tx_comms.h"

void setup(){
    // for receiving
    start_snailESPNOW_serial();
    
    // for sending
    //send_ESPNOW_init();
}

void loop(){

    // for sending
    //ESPNOW_loop();
    //Serial.println("ESP32 loop running...");
    //comms_tester();
    //delay(4); // 250Hz max
}

    