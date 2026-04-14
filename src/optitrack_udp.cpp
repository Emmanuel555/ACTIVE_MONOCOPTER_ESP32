#include <Arduino.h>
#include "optitrack_udp.h"

OptitrackUDP optitrack;

// ---------------------------------------------------------------
// begin() — bind socket and compute sample rate from N packets
// Call this after WiFi is connected (i.e. after connectToWiFi()).
// ---------------------------------------------------------------
void OptitrackUDP::begin(int bodies) {
    num_bodies = bodies;
    sock.begin(OPTITRACK_PORT);
    Serial.printf("OptiTrack UDP listening on port %u\n", OPTITRACK_PORT);

    Serial.println("Computing OptiTrack sample rate...");

    int      pkt_size = BYTES_PER_BODY * num_bodies;
    uint8_t  tmp[BYTES_PER_BODY * MAX_BODIES];
    uint32_t timestamps[SAMPLE_RATE_N];

    for (int i = 0; i < SAMPLE_RATE_N; i++) {
        int sz = 0;
        while ((sz = sock.parsePacket()) < pkt_size) {
            if (sz > 0)
                Serial.printf("[optitrack] unexpected packet size: %d (expected %d)\n", sz, pkt_size);
        }
        sock.read(tmp, pkt_size);
        timestamps[i] = micros();
        if (i == 0) Serial.println("[optitrack] first packet received!");
    }

    float sum = 0.0f;
    for (int i = 1; i < SAMPLE_RATE_N; i++) {
        sum += (float)(timestamps[i] - timestamps[i - 1]);
    }
    float mean_us = sum / (SAMPLE_RATE_N - 1);
    sample_time   = mean_us * 1e-6f;
    sample_rate   = 1.0f / sample_time;

    Serial.printf("Sample rate: %.2f Hz\n", sample_rate);

    // seed data with the last calibration packet
    memcpy(&data, tmp, BYTES_PER_BODY);
}

// ---------------------------------------------------------------
// get_data() — blocking receive, mirrors Python get_data().
// Waits until a packet arrives, then parses 7 x int16 into data.
// Returns true on success.
// ---------------------------------------------------------------
bool OptitrackUDP::get_data() {
    int pkt_size = BYTES_PER_BODY * num_bodies;
    while (sock.parsePacket() < pkt_size) { /* spin */ }
    sock.read((uint8_t*)&data, pkt_size);
    return true;
}

// ---------------------------------------------------------------
// send_to_teensy() — sends scaled floats over UART (Serial1)
// Format: "x,y,z,qx,qy,qz,qw\n"  — parse with strtok on Teensy
// ---------------------------------------------------------------
void OptitrackUDP::send_to_teensy(float x, float y, float z,
                                   float qx, float qy, float qz, float qw) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
             x, y, z, qx, qy, qz, qw);
    Serial1.print(buf);
}
