#include <WiFi.h>
#include <WiFiUdp.h>
#include "snail_rx_comms.h"
#include <esp_now.h>

// ESP32 UART pins connected to Teensy
// change these to match your wiring
//constexpr int RXD2 = 44;
//constexpr int TXD2 = 43;

int snail_packetCount = 0;
unsigned long snail_lastPrint = 0;
int no_of_elements = 5; // number of PWM values expected in each packet
state_estimation states;


void snail_ESPNOW_init() {
  Serial.print("Connecting via ESPNOW");
  WiFi.mode(WIFI_STA);
  // print MAC address so you can copy it to desk ESP32
  Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
  esp_now_init();
  esp_now_register_recv_cb(snail_onReceive);
}

//void print_mac_address(){
//    Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
//}

void start_snailESPNOW_serial() {
  Serial.begin(115200);
  delay(200);  
  Serial.println("ESP32 starting...");                       // USB debug
  //Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART to Teensy
  
  //delay(1000);
  snail_ESPNOW_init();

  // warmup Serial1 to Teensy - put this last
  unsigned long start = millis();
  while (millis() - start < 1000) {
      Serial1.println(1000);
  }

  Serial.println("ESP32 RX ready for ESPNOW..");

  // optional: announce to Teensy
  //Serial1.println("ESP NOW ready");
}


void snail_onReceive(const uint8_t *mac, const uint8_t *data, int len) {
    // flush any queued Serial1 data first, this is primarily for teensy stuff...nth wireless
    while (Serial1.available()) {
        Serial1.read();
    }

    String msg = "";
    for (int i = 0; i < len; i++) {
        msg += (char)data[i];
    }
    
    msg.trim();

    snail_packetCount++;
    
    if (millis() - snail_lastPrint >= 1000) {
        Serial.printf("ESP-NOW packets/sec: %d\n", snail_packetCount);
        snail_packetCount = 0;
        snail_lastPrint = millis();
    }
    

    /* // for simple PWM
    int pwm = msg.toInt();
    pwm = constrain(pwm, 1000, 2000);

    //Serial.printf("Received PWM: %d\n", pwm); 
    
    // forward to Teensy
    Serial1.print(pwm);
    Serial1.print('\n');   */

    // receive 5 floats (4 bytes each, 20 bytes total)
    if (len >= no_of_elements * 4) {
        memcpy(&states.x,   data,      4);
        memcpy(&states.y,   data + 4,  4);
        memcpy(&states.vx,  data + 8,  4);
        memcpy(&states.vy,  data + 12, 4);
        memcpy(&states.mag, data + 16, 4);

        //Serial.printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", states.x, states.y, states.vx, states.vy, states.mag);
    }

    send_states(states);

}


void send_states(state_estimation states) {
    Serial.printf("%.3f,%.3f,%.3f,%.3f,%.3f\n", 
        states.x, states.y, states.vx, states.vy, states.mag);
}
