/**
 * LDRDevice.h
 **/

#ifndef LDRDEVICE_H
#define LDRDEVICE_H

#include <Arduino.h>
#include "Device.h"

// LDR sensor values. The lower value is the brighter is light.
const uint8_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

class LDRDevice: public Device {
    public:

        LDRDevice(uint8_t pin) {
            this->_pin = pin;
            uint16_t lightVal = getLightValue();
            this->isPowerOn = lightVal < LAMP_ON_VALUE ? true : false;
         };

        void lampStatus();

        String status();

    private:

        uint16_t getLightValue();

};

#endif