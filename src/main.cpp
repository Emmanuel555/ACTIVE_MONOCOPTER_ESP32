#include <Arduino.h>

// const int TEST_PIN = 8;   // change this to the GPIO you want to test

//#define RXD2 8
//#define TXD2 7

// for some reason, the gpio below follows the pin numbering of the ESP32-S3-DevKitC-1, which is different from the Schematics.
static const int RXD2 = 44;
static const int TXD2 = 43;  

void setup(){
    Serial.begin(115200);  // initialize serial connection to PC monitor
    
    // Give PC time to open up the serial connection/monitor before we start sending data, otherwise the first few messages may be missed.
    delay(5000);

    //Serial.println("Starting GPIO output test");
    //pinMode(TEST_PIN, OUTPUT);  // set the test pin as an output

    Serial.println("ESP32 starting...");

    // Now safe to init UART
    // UART to Teensy
    Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(1000);   
}

void loop(){
    
    Serial1.println("hello");

    delay(50);

    if (Serial1.available()) {
        //Serial.println("Printing: ");
        char c = Serial1.read();
        Serial.write(c);
    }
    
    delay(50);
}
    
    
    
    //digitalWrite(TEST_PIN, HIGH);
    //Serial.println("HIGH");
    //delay(1000);

    //digitalWrite(TEST_PIN, LOW);
    //Serial.println("LOW");
    //delay(1000);




// Send message
    // Serial.println("hello");

    // // Wait a bit
    // delay(500);

    // // Read response
    // while (Serial2.available()) {
    //     char c = Serial2.read();
    //     Serial.print(c);  // should print "hello"
    // }

    // Serial.println();
    //delay(1000);