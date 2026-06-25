#include <WiFi.h>
#include <WiFiUdp.h>
#include "snail_rx_comms.h"
#include <esp_now.h>

using namespace std;

// ESP32 UART pins connected to Teensy
// change these to match your wiring
//constexpr int RXD2 = 44;
//constexpr int TXD2 = 43;

int snail_packetCount = 0;
unsigned long snail_lastPrint = 0;
int no_of_elements = 5; // number of PWM values expected in each packet
state_estimation states;


void snail_ESPNOW_init(String mode) {
  Serial.print("Connecting via ESPNOW.....");
  WiFi.mode(WIFI_STA);
  // print MAC address so you can copy it to desk ESP32
  Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
  Serial.printf("WiFi channel: %d\n", WiFi.channel());
  esp_err_t initResult = esp_now_init();
  Serial.printf("esp_now_init result: %s (mode=%s)\n", initResult == ESP_OK ? "OK" : "FAILED", mode.c_str());

  if (mode == "teensy") {
      esp_now_register_recv_cb(snail_onReceive_5f);
  } else if (mode == "com") {
      esp_now_register_recv_cb(snail_onReceive);
  } else {
      Serial.printf("WARNING: mode \"%s\" matched neither \"teensy\" nor \"com\" - no recv callback registered!\n", mode.c_str());
  }

}

//void print_mac_address(){
//    Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
//}

void start_snailESPNOW_serial(String mode) {
  Serial.begin(115200);
  delay(200);  
  Serial.println("ESP32 starting...");                       // USB debug
  //Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART to Teensy
  
  delay(1000);
  snail_ESPNOW_init(mode);

  Serial.println("ESP32 RX ready for ESPNOW..");

  // optional: announce to Teensy
  //Serial1.println("ESP NOW ready");
}


void snail_onReceive(const uint8_t *mac, const uint8_t *data, int len) {
    // flush any queued Serial1 data first, this is primarily for teensy stuff...nth wireless
    //while (Serial1.available()) {
    //    Serial1.read();
    //}

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

    //send_states(states);

}


void send_states(state_estimation states) {
    Serial.printf("%.3f,%.3f,%.3f,%.3f,%.3f\n",
        states.x, states.y, states.vx, states.vy, states.mag);
}


void snail_onReceive_5f(const uint8_t *mac, const uint8_t *data, int len) {
    if (len < 5 * 4) return;

    actuator_cmd5 cmd;
    memcpy(&cmd.values[0], data,      4);
    memcpy(&cmd.values[1], data + 4,  4);
    memcpy(&cmd.values[2], data + 8,  4);
    memcpy(&cmd.values[3], data + 12, 4);
    memcpy(&cmd.values[4], data + 16, 4);

    Serial.printf("Received actuator cmd: %.3f,%.3f,%.3f,%.3f,%.3f\n",
        cmd.values[0], cmd.values[1], cmd.values[2], cmd.values[3], cmd.values[4]);

    send_to_teensy_5f(cmd);
}


void send_to_teensy_5f(actuator_cmd5 cmd) {
    Serial1.write((uint8_t *)&cmd, sizeof(cmd));
}
