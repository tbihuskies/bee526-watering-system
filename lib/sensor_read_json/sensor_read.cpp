// sensor_read.cpp

#include <Arduino.h>
#include "sensor_read.h"

using namespace WaterSoil; 

    void readSensor(int pin) {
        int sensorReading = analogRead(pin);
        Serial.print("Moisture reading is: ");
        Serial.println(sensorReading);
        delay(5000);

    }

