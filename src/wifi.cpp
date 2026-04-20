#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_rec.h"
#include <esp_now.h>


// PCB ESP32 MAC address - get this from PCB ESP32's Serial monitor
uint8_t pcbMacAddress[] = {0xD8, 0x3B, 0xDA, 0x45, 0x88, 0x18};

esp_now_peer_info_t peerInfo;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
Serial.printf("Send status: %s\n", status == ESP_NOW_SEND_SUCCESS ? "SENT (RX Online)" : "FAILED (RX Offline)");
}


void send_ESPNOW_init() {
    Serial.begin(115200);
    setCpuFrequencyMhz(80);          // reduce CPU speed
    //WiFi.setTxPower(WIFI_POWER_2dBm); // reduce TX power
    WiFi.mode(WIFI_STA);
    esp_now_init();
    //esp_now_register_send_cb(onSent); // too much overhead for this callback, it causes significant latency spikes, so we will just check status in loop instead

    memcpy(peerInfo.peer_addr, pcbMacAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Peer added successfully");
}


void ESPNOW_loop() {
    /* if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        Serial.println("Reading from PC Serial: " + msg);
        msg.trim();
        esp_now_send(pcbMacAddress, (uint8_t*)msg.c_str(), msg.length());
    } */

    if (Serial.available() >= 2) {
        uint8_t data[2];
        Serial.readBytes(data, 2);
        esp_now_send(pcbMacAddress, data, 2);
    }
}


void comms_tester() {
  String testMsg = "Computer sending paused - ESP32 alive";
  esp_now_send(pcbMacAddress, (uint8_t*)testMsg.c_str(), testMsg.length());
}






















const char* ssid = "Free";
const char* password = "748748748";

WiFiUDP udp;

const unsigned int OptiPort = 22222;
const unsigned int localPort = 5005;
IPAddress pcIP(192, 168, 1, 146);   // replace with your PC IP
const unsigned int pcPort = 5005;

// ESP32 UART pins connected to Teensy
// change these to match your wiring
constexpr int RXD2 = 44;
constexpr int TXD2 = 43;

String teensyMsg = "";
char incomingPacket[256];


IPAddress local_IP(192, 168, 1, 108);  // pick a free IP on your network
IPAddress gateway(192, 168, 1, 1);      // your router
IPAddress subnet(255, 255, 255, 0);   // subnet mask


void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}


void ESPNOW_init() {
  Serial.print("Connecting via ESPNOW");
  WiFi.mode(WIFI_STA);
  // print MAC address so you can copy it to desk ESP32
  Serial.printf("Receiver MAC: %s\n", WiFi.macAddress().c_str());
  
  esp_now_init();
  //esp_now_register_recv_cb(onReceive);
}


void sendUDPMessage(const String& msg) {
  udp.beginPacket(pcIP, pcPort);
  udp.print(msg);
  udp.endPacket();
}


void start_wifi_serial() {
  Serial.begin(115200);
  delay(200);  
  Serial.println("ESP32 starting...");                       // USB debug
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART to Teensy
  
  //delay(1000);
  connectToWiFi();

  udp.begin(pcPort);
  Serial.printf("UDP listening on port %u\n", pcPort);
  Serial.println("ESP32 ready for MOTOR CONTROL..");

  // announce to PC first, thats for print comms
  //sendUDPMessage("ESP32 UDP bridge ready");
  //Serial.println("Sent startup packet to PC");

  // optional: announce to Teensy
  //Serial1.println("ESP32 ready");
}


void start_ESPNOW_serial() {
  Serial.begin(115200);
  delay(200);  
  Serial.println("ESP32 starting...");                       // USB debug
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART to Teensy
  
  //delay(1000);
  ESPNOW_init();

  Serial.println("ESP32 ready for ESPNOW..");

  // optional: announce to Teensy
  //Serial1.println("ESP NOW ready");
}


void wifi_recursion () {
  // =========================
  // PC -> ESP32 -> Teensy
  // =========================
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }

    Serial.print("From PC: ");
    Serial.println(incomingPacket);
    Serial1.println(incomingPacket);   // forward to Teensy

  }

  // =========================
  // Teensy -> ESP32 -> PC
  // =========================
  
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      Serial.print("From Teensy: ");
      Serial.println(teensyMsg);

      sendUDPMessage(teensyMsg);   // forward to PC
      teensyMsg = "";
    } else if (c != '\r') {
      teensyMsg += c;
    }
  }
}


void motor_wifi_recursion () {
  // =========================
  // PC -> ESP32 -> Teensy
  // =========================
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }

    int pwm_value = atoi(incomingPacket); // convert string to int
    pwm_value = constrain(pwm_value, 1000, 2000); // clamp just in case

    Serial.printf("From PC: %d\n", pwm_value);
    Serial1.println(pwm_value);   // forward to Teensy

  }

  // =========================
  // Teensy -> ESP32 -> PC
  // =========================
  
  /* while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      Serial.printf("From Teensy: %s\n", teensyMsg.c_str());
      sendUDPMessage(teensyMsg);   // forward to PC
      teensyMsg = "";
    } else if (c != '\r') {
      teensyMsg += c;
    }
  } */
}


void dshot_motor_wifi_recursion () {
  // =========================
  // PC -> ESP32 -> Teensy
  // =========================
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }

    // validate comma exists before forwarding
    String msg = String(incomingPacket);
    int commaIndex = msg.indexOf(',');
    if (commaIndex != -1) {
        int direction = msg.substring(0, commaIndex).toInt();
        float value = msg.substring(commaIndex + 1).toFloat();

        //Serial.printf("From PC - Direction: %d, Value: %.2f\n", direction, value);
        Serial.printf("Forwarding to Teensy: %s\n", msg);
        Serial1.println(msg);   // forward raw string to Teensy
    } else {
        Serial.println("Invalid packet format, expected direction,value");
    }

  }

  // =========================
  // Teensy -> ESP32 -> PC
  // =========================
  
  /* while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      Serial.printf("From Teensy: %s\n", teensyMsg.c_str());
      sendUDPMessage(teensyMsg);   // forward to PC
      teensyMsg = "";
    } else if (c != '\r') {
      teensyMsg += c;
    }
  } */
}


void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
    String msg = "";
    for (int i = 0; i < len; i++) {
        msg += (char)data[i];
    }
    msg.trim();

    int commaIndex = msg.indexOf(',');
    if (commaIndex != -1) {
        String dirStr = msg.substring(0, commaIndex);
        String valStr = msg.substring(commaIndex + 1);
        dirStr.trim();
        valStr.trim();

        Serial.printf("From desk ESP32: %s\n", msg.c_str());
        Serial1.println(msg); // forward to Teensy
    }
}