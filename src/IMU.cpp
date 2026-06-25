#include <goBILDA_Pinpoint.h>
#include "IMU.h"
#include <Wire.h>       // <-- Required for I2C

goBILDA::Pinpoint odom;

void start_imu() {
    odom.begin();                  // begin() does its own internal Wire.begin() on default pins
    Wire.end();                    // tear down so the next begin() actually re-applies new pins
    Wire.begin(9, 8, 100000);      // reassert our actual I2C pins/clock so they're what's actually used
    delay(3000);

    // NOTE: getDeviceStatus()/PinpointStatus is unreliable in this library - its
    // fields are left uninitialized except on the specific bit-set paths that
    // explicitly assign them. bulkRead().Error is properly initialized and set
    // by actual communication logic, so gate on that instead.
    goBILDA::BulkReadData data = odom.bulkRead();
    while (data.Error != goBILDA::PinpointError::None) {
        Serial.printf("Pinpoint not detected! error=%d\n", (int)data.Error);
        delay(100);
        data = odom.bulkRead();
    }

    Serial.println("Pinpoint ready!");

    // Example configuration:
    odom.setEncoderResolution(19.894f);
    odom.setOffsets(-84, -168);
    odom.setEncoderDirections(goBILDA::EncoderDirection::Backward, goBILDA::EncoderDirection::Backward);

    odom.resetPositionAndIMU();
}


IMUDATA getPWMValues() {
    IMUDATA DATA;

    DATA.posX = 0;
    DATA.posY = 0;
    DATA.velX = 0;
    DATA.velY = 0;
    DATA.heading = 0;

    if (Serial.available()) {
        char c = Serial.read();

        if (c == 'i' || c == 'I') {
            Serial.println("Recalibrating IMU (no position reset)...");
            odom.recalibrateIMU();
            delay(300);  // IMU needs ~0.25 sec
            Serial.println("IMU recalibration complete.");
        }

        if (c == 'p' || c == 'P') {
            Serial.println("Resetting position and recalibrating IMU...");
            odom.resetPositionAndIMU();
            delay(300);  // IMU needs ~0.25 sec
            Serial.println("Position reset + IMU recalibration complete.");
        }
    }

    goBILDA::BulkReadData data = odom.bulkRead();
    if (data.Error == goBILDA::PinpointError::None) {
        float headingDeg = data.Position.heading * (180.0 / PI);
        float headingVelDeg = data.VelocityH * (180.0 / PI);

        DATA.posX = data.Position.x;
        DATA.posY = data.Position.y;
        DATA.velX = data.VelocityX;
        DATA.velY = data.VelocityY;
        DATA.heading = data.Position.heading;

        //Serial.printf("X=%.2f  Y=%.2f  H=%.2f  VelX (mm/s)=%.2f  VelY (mm/s)=%.2f  VelH (deg/s)=%.2f\n",
        //    DATA.posX, DATA.posY, headingDeg, DATA.velX, DATA.velY, headingVelDeg);
        return DATA;
    }

    Serial.println("Failed to read Pinpoint data");
    delay(20);
    return DATA;
}
