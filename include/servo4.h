#pragma once

// pin assignments (GPIO numbers, Xiao ESP32S3)
constexpr int SERVO_ANGLE1_PIN = 1;
constexpr int SERVO_ANGLE2_PIN = 2;

//constexpr int SERVO_CONT1_PIN  = 6;
//constexpr int SERVO_CONT2_PIN  = 43;

constexpr int SERVO_CONT1_PIN  = 7;
constexpr int SERVO_CONT2_PIN  = 44;

void servo4_init();
void servo4_writeAngles(int deg1, int deg2);        // pins 1, 2 — degrees 0-180
void servo4_writeContinuous(int us1, int us2);      // pins 6, 43 — microseconds, 1500 = stop
