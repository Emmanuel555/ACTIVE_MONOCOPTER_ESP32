#include <Arduino.h>
#include "snail_rx_comms.h"
#include "snail_tx_comms.h"
#include <WiFi.h>
#include "IMU.h"

String mode = "teensy"; // "teensy" or "com" - set this to switch between receiving from Teensy or computer 

void setup(){
    // for receiving - this also calls Serial.begin(), so it must run before
    // anything that prints (e.g. teensy_start()) or those prints are silently dropped
    start_snailESPNOW_serial(mode); //

    if (mode == "teensy") {
        start_imu(); // initialize IMU and encoders
        teensy_start(); // initialize Serial1 to Teensy
        uint8_t rxMac[] = {0xD8, 0x3B, 0xDA, 0x45, 0x87, 0xE0};
        setSnailMacAddress(rxMac); // set RX MAC address for Teensy mode
    }
    else {
        uint8_t rxMac[] = {0x90, 0x70, 0x69, 0x12, 0x13, 0xBC};
        setSnailMacAddress(rxMac); // set RX MAC address for computer mode
    }

    // for sending
    send_ESPNOW_init_lite(); //
}


void loop(){
    if (mode == "teensy") {
        // read IMU and encoders
        IMUDATA imuData = getPWMValues();
        feedback_states(imuData.posX, imuData.posY, imuData.velX, imuData.velY, imuData.heading);
    }
    /* else {
        control_loop(); 
    } */

    //Serial.println("ESP32 loop running..."); //
    //Serial.printf("Robot MAC: %s\n", WiFi.macAddress().c_str());
    //comms_tester();
    //delay(4); // 250Hz max
}

    