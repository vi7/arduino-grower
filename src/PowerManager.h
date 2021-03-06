/**
 * PowerManager.h
 **/

#ifndef POWERMANAGER_H
#define POWERMANAGER_H

/* normally closed (NC) relay NC defines */
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#include <Arduino.h>
#include <BlynkSimpleEsp8266_SSL.h>

class PowerManager {
    public:
        
        static void autoPower(bool *autoControl, bool *isOn, float *currVal, float maxVal, float valHyst, uint8_t pin, WidgetLED *led);

        static bool manualPower(bool enabled, uint8_t pin, WidgetLED *led);

};


#endif