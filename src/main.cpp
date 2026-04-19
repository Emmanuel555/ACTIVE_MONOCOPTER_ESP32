#include <Arduino.h>
//#include "comms.h"
#include "protocols.h"
#include "optitrack_udp.h"

void setup(){
    start_ESPNOW_serial();
    
    //start_wifi_serial();
    //optitrack.begin();
    //Serial.begin(115200);
    //delay(3000);
    //Serial.println("BOOT OK");
}

void loop(){
    /* optitrack.get_data();
    float x  = optitrack.data.x  * 0.0005f;
    float y  = optitrack.data.y  * 0.0005f;
    float z  = optitrack.data.z  * 0.0005f;
    float qx = optitrack.data.qx / 1000.0f;
    float qy = optitrack.data.qy / 1000.0f;
    float qz = optitrack.data.qz / 1000.0f;
    float qw = optitrack.data.qw / 1000.0f;

    Serial.printf("pos=(%.3f,%.3f,%.3f) quat=(%.3f,%.3f,%.3f,%.3f) Update rate = (%.3f)\n",
        x, y, z, qx, qy, qz, qw, optitrack.sample_rate);

    optitrack.send_to_teensy(x, y, z, qx, qy, qz, qw); */
    //motor_wifi_recursion();

}

    