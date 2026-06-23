#include <Arduino.h>
#include "snail_rx_comms.h"
#include "snail_tx_comms.h"
#include <WiFi.h>
#include "IMU.h"

String mode = "com"; // "teensy" or "com" - set this to switch between receiving from Teensy or computer 

void setup(){   
    if (mode == "teensy") {
        start_imu(); // initialize IMU and encoders
        teensy_start(); // initialize Serial1 to Teensy
        uint8_t rxMac[] = {0xD8, 0x3B, 0xDA, 0x45, 0x87, 0x18};
        setSnailMacAddress(rxMac); // set RX MAC address for Teensy mode
    }
    else {
        uint8_t rxMac[] = {0xD8, 0x3B, 0xDA, 0x45, 0x88, 0x18};
        setSnailMacAddress(rxMac); // set RX MAC address for computer mode
    }
    
    // for receiving
    start_snailESPNOW_serial(mode); //

    // for sending
    send_ESPNOW_init_lite(); //    
}


void loop(){
    if (mode == "teensy") {
        // read IMU and encoders
        IMUDATA imuData = getPWMValues();
        feedback_states(imuData.posX, imuData.posY, imuData.velX, imuData.velY, imuData.heading);
    }
    else {
        control_loop(); 
    }

    //Serial.println("ESP32 loop running..."); //
    //Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
    //comms_tester();
    //delay(4); // 250Hz max
}

    