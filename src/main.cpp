#include <Arduino.h>

void setup(){
    Serial.begin(115200);     // PC monitor
    
    // Give USB time
    delay(1000);

    Serial.println("ESP32 starting...");

    // Now safe to init UART
    // Serial2.begin(115200);
}

void loop(){
    // Send message
    Serial.println("hello");

    // // Wait a bit
    // delay(500);

    // // Read response
    // while (Serial2.available()) {
    //     char c = Serial2.read();
    //     Serial.print(c);  // should print "hello"
    // }

    // Serial.println();
    delay(1000);
}