/**
 * PowerManager.h
 **/

#ifndef POWERMANAGER_H
#define POWERMANAGER_H

/* normally closed (NC) relay NC defines */
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include <Arduino.h>
#include <RCSwitch.h>

#include "Settings.h"

class PowerManager {
    public:
        static RCSwitch transmitter;
        #ifdef RADIO_POWER
        static void autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst,
                                uint32_t *onCode, uint32_t *offCode);
        static bool manualPowerOn(uint32_t *onCode);
        static bool manualPowerOff(uint32_t *offCode);
        #else
        static void autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst, uint8_t *pin);
        static bool manualPowerOn(uint8_t pin);
        static bool manualPowerOff(uint8_t pin);
        #endif

};

#endif