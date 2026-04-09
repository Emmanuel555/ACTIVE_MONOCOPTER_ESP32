#pragma once

#include <WiFiUdp.h>

#define OPTITRACK_PORT     22222
#define BYTES_PER_BODY     14        // 7 x int16 per body
#define SAMPLE_RATE_N      100       // number of packets used to estimate sample rate
#define MAX_BODIES         3

struct OptitrackData {
    int16_t x, y, z;         // position
    int16_t qx, qy, qz, qw;  // quaternion
};

struct OptitrackUDP {
    WiFiUDP       sock;
    OptitrackData data;              // parsed packet (1 body)
    int           num_bodies;
    float         sample_rate;       // Hz, computed after init
    float         sample_time;       // seconds

    void          begin(int bodies = 1);
    bool          get_data();        // blocking receive — returns true on success
};

extern OptitrackUDP optitrack;
