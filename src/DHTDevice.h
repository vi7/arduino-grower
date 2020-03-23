/**
 * DHTDevice.h
 **/

#ifndef DHTDEVICE_H
#define DHTDEVICE_H

#include <Arduino.h>
#include <DHTesp.h>
#include "Device.h"
#include "BlynkManager.h"

class DHTDevice: public Device {

    public:
        uint16_t blynkTempPin, blynkRhPin;
        uint16_t dhtReadInterval;
        float temp, rH;

        void init(uint8_t pin, uint8_t blynkTempPin);

        void init(uint8_t pin, uint8_t blynkTempPin, uint8_t blynkRhPin); 

        void tempDataHandler(Device device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void rhDataHandler(Device device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void powerOn();

        void powerOff();

        String status();

    private:
        DHTesp dht;
};

#endif