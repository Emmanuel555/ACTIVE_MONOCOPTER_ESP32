#include "tof_sensors.h"

#include <Wire.h>
#include <VL6180X.h>
#include <math.h>

static VL6180X sensors[TOF_NUM_SENSORS];
static bool    sensorOk[TOF_NUM_SENSORS] = {false};

// Check whether a device ACKs at the given I2C address
static bool i2cAnswers(uint8_t addr) {
    Wire.beginTransmission(addr);
    return Wire.endTransmission() == 0;
}

// Read all sensors strictly one at a time. Time taken at start of sweep.
static uint32_t readSweep(int16_t out[TOF_NUM_SENSORS]) {
    uint32_t t = millis();
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        out[i] = readSensor(i);
    }
    return t;
}

uint8_t initSensors() {
    Wire.begin(TOF_SDA_PIN, TOF_SCL_PIN);
    Wire.setClock(TOF_I2C_CLOCK_HZ);

    // All sensors off
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        pinMode(TOF_CE_PINS[i], OUTPUT);
        digitalWrite(TOF_CE_PINS[i], LOW);
    }
    delay(10);

    uint8_t okCount = 0;
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        uint8_t newAddr = TOF_BASE_ADDR + i;
        sensorOk[i] = false;

        // Wake this sensor only; it comes up at 0x29
        digitalWrite(TOF_CE_PINS[i], HIGH);
        delay(2);

        if (i2cAnswers(TOF_DEFAULT_ADDR)) {
            sensors[i].setBus(&Wire);
            sensors[i].init();
            sensors[i].configureDefault();
            sensors[i].setAddress(newAddr);
            sensors[i].setTimeout(TOF_TIMEOUT_MS);

            // init() returns void, so verify via model ID at the new address
            if (i2cAnswers(newAddr) &&
                sensors[i].readReg(VL6180X::IDENTIFICATION__MODEL_ID) == 0xB4) {
                sensorOk[i] = true;
            }
        }

        if (sensorOk[i]) {
            okCount++;
            Serial.printf("# sensor %d (CE pin %d): OK at 0x%02X\n",
                          i, TOF_CE_PINS[i], newAddr);
        } else {
            // Turn it back off so it can't clash at 0x29 with the next sensor
            digitalWrite(TOF_CE_PINS[i], LOW);
            Serial.printf("# sensor %d (CE pin %d): FAILED, skipping\n",
                          i, TOF_CE_PINS[i]);
        }
    }

    Serial.printf("# %d/%d sensors OK\n", okCount, TOF_NUM_SENSORS);
    return okCount;
}

int16_t readSensor(uint8_t i) {
    if (i >= TOF_NUM_SENSORS || !sensorOk[i]) return TOF_INVALID;

    uint16_t raw = sensors[i].readRangeSingleMillimeters();
    if (sensors[i].timeoutOccurred()) return TOF_INVALID;

    uint8_t status = sensors[i].readReg(VL6180X::RESULT__RANGE_STATUS) >> 4;
    if (status != 0) return TOF_INVALID;
    if (raw == 255)  return TOF_INVALID;

    return (int16_t)lroundf(raw * TOF_SCALE[i] + TOF_OFFSET_MM[i]);
}

void printHeader() {
    Serial.println("time_ms,s0_mm,s1_mm,s2_mm,s3_mm,s4_mm");
}

void printSweep() {
    int16_t r[TOF_NUM_SENSORS];
    uint32_t t = readSweep(r);

    Serial.printf("%lu", (unsigned long)t);
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        Serial.printf(",%d", r[i]);
    }
    Serial.println();
}

void printReadings() {
    int16_t r[TOF_NUM_SENSORS];
    uint32_t t = readSweep(r);

    Serial.printf("t=%lu ms", (unsigned long)t);
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        Serial.printf("  s%d: %d", i, r[i]);
    }
    Serial.println();
}

void buildPacket(uint8_t out[TOF_PACKET_SIZE]) {
    int16_t r[TOF_NUM_SENSORS];
    uint32_t t = readSweep(r);

    int k = 0;
    out[k++] = TOF_SYNC_BYTE;

    // uint32 time_ms, little-endian
    out[k++] = (uint8_t)(t);
    out[k++] = (uint8_t)(t >> 8);
    out[k++] = (uint8_t)(t >> 16);
    out[k++] = (uint8_t)(t >> 24);

    // int16 per sensor, little-endian, -1 = invalid
    for (int i = 0; i < TOF_NUM_SENSORS; i++) {
        int16_t v = r[i];
        out[k++] = (uint8_t)(v & 0xFF);
        out[k++] = (uint8_t)((v >> 8) & 0xFF);
    }
}

// Note: Serial1 must be started (Serial1.begin(...)) before calling this.
void sendReadingsSerial1() {
    uint8_t pkt[TOF_PACKET_SIZE];
    buildPacket(pkt);
    Serial1.write(pkt, TOF_PACKET_SIZE);
}
