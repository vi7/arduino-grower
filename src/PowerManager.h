/**
 * PowerManager.h
 **/

#ifndef POWERMANAGER_H
#define POWERMANAGER_H

/* normally closed (NC) relay NC defines */
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include <Arduino.h>

class PowerManager {
    public:
        static void autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst, uint8_t *pin);
        static bool manualPowerOn(uint8_t pin);
        static bool manualPowerOff(uint8_t pin);

};

#endif