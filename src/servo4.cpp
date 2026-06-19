#include "servo4.h"
#include <ESP32Servo.h>

namespace {
    Servo servos[4];
    const int servoPins[4] = {SERVO_ANGLE1_PIN, SERVO_ANGLE2_PIN, SERVO_CONT1_PIN, SERVO_CONT2_PIN};
}


void servo4_init() {
    // Each attach() otherwise grabs the next free LEDC timer on demand;
    // on the S3 (4 timers total) that runs out after 2-3 attaches and the
    // earlier-defined Servo objects silently fail to attach. Reserving all
    // 4 timers up front before any attach() call fixes that.
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    for (int i = 0; i < 4; i++) {
        servos[i].setPeriodHertz(50);
        servos[i].attach(servoPins[i], 1000, 2000);
    }

    // attach() drives the pin with a default pulse before write()/writeMicroseconds()
    // is ever called; for the continuous-rotation servos (index 2, 3) that default
    // isn't guaranteed to land on this unit's true stop point, so force it explicitly.
    servos[2].writeMicroseconds(1500);
    servos[3].writeMicroseconds(1500);
}


void servo4_writeAngles(int deg1, int deg2) {
    servos[0].write(constrain(deg1, 0, 180));
    servos[1].write(constrain(deg2, 0, 180));
    Serial.println("Servo angles updated....");
}


void servo4_writeContinuous(int us1, int us2) {
    servos[2].writeMicroseconds(constrain(us1, 1000, 2000));
    servos[3].writeMicroseconds(constrain(us2, 1000, 2000));
    Serial.println("Servo continuous values updated....");
}
