#include <WiFi.h>
#include <WiFiUdp.h>
#include "wifi_rec.h"

const char* ssid = "Free";
const char* password = "748748748";

WiFiUDP udp;

const unsigned int localPort = 5005;
IPAddress pcIP(192, 168, 1, 146);   // replace with your PC IP
const unsigned int pcPort = 5005;

// ESP32 UART pins connected to Teensy
// change these to match your wiring
constexpr int RXD2 = 44;
constexpr int TXD2 = 43;


String teensyMsg = "";
char incomingPacket[256];


void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
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

void sendUDPMessage(const String& msg) {
  udp.beginPacket(pcIP, pcPort);
  udp.print(msg);
  udp.endPacket();
}

void start_wifi_serial() {
  Serial.begin(115200);
  delay(5000);  
  Serial.println("ESP32 starting...");                       // USB debug
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2); // UART to Teensy
  
  //delay(1000);
  connectToWiFi();

  udp.begin(localPort);
  Serial.printf("UDP listening on port %u\n", localPort);

  // announce to PC first
  sendUDPMessage("ESP32 UDP bridge ready");
  Serial.println("Sent startup packet to PC");

  // optional: announce to Teensy
  Serial1.println("ESP32 ready");
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

    delay(50);
    if (Serial1.available()) {
      Serial.println("Serial1 available");
    }
    else {
      Serial.println("Serial1 NOT available");
    }
    delay(50);

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