#pragma once

#include <Arduino.h>

void start_imu();

struct IMUDATA {
  float posX; 
  float posY;
  float velX;
  float velY;
  float heading;
};

IMUDATA getPWMValues();