#include "servo4.h"
#include <Arduino.h>

namespace {
    constexpr int FREQ_HZ = 50;
    constexpr int RES_BITS = 16;
    constexpr uint32_t PERIOD_US = 1000000UL / FREQ_HZ;
    constexpr uint32_t MAX_DUTY = (1UL << RES_BITS) - 1;

    constexpr int CH_ANGLE1 = 0;
    constexpr int CH_ANGLE2 = 1;
    constexpr int CH_CONT1  = 2;
    constexpr int CH_CONT2  = 3;

    void writePulseUs(int channel, int us) {
        uint32_t duty = (uint32_t)((uint64_t)us * MAX_DUTY / PERIOD_US);
        ledcWrite(channel, duty);
    }
}

void servo4_init() {
    ledcSetup(CH_ANGLE1, FREQ_HZ, RES_BITS);
    ledcSetup(CH_ANGLE2, FREQ_HZ, RES_BITS);
    ledcSetup(CH_CONT1,  FREQ_HZ, RES_BITS);
    ledcSetup(CH_CONT2,  FREQ_HZ, RES_BITS);

    ledcAttachPin(SERVO_ANGLE1_PIN, CH_ANGLE1);
    ledcAttachPin(SERVO_ANGLE2_PIN, CH_ANGLE2);
    ledcAttachPin(SERVO_CONT1_PIN,  CH_CONT1);
    ledcAttachPin(SERVO_CONT2_PIN,  CH_CONT2);

    writePulseUs(CH_ANGLE1, 1500);
    writePulseUs(CH_ANGLE2, 1500);
    writePulseUs(CH_CONT1, 1500);
    writePulseUs(CH_CONT2, 1500);
}

void servo4_writeAngles(int deg1, int deg2) {
    deg1 = constrain(deg1, 0, 180);
    deg2 = constrain(deg2, 0, 180);
    writePulseUs(CH_ANGLE1, map(deg1, 0, 180, 1000, 2000));
    writePulseUs(CH_ANGLE2, map(deg2, 0, 180, 1000, 2000));
}

void servo4_writeContinuous(int us1, int us2) {
    writePulseUs(CH_CONT1, constrain(us1, 1000, 2000));
    writePulseUs(CH_CONT2, constrain(us2, 1000, 2000));
}
