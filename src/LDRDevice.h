/**
 * LDRDevice.h
 **/

#ifndef LDRDEVICE_H
#define LDRDEVICE_H

#include <Arduino.h>
#include "Device.h"
#include "BlynkManager.h"

// LDR sensor values. The lower value is the brighter is light.
const uint8_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

class LDRDevice: public Device {
    public:
        void init(uint8_t pin, uint8_t blynkPin);

        void lampStatus();

        void powerOn();

        void powerOff();

        String status();
        
    private:

        uint16_t getLightValue();

};

#endif