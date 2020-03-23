/**
 * BlynkManager.h
 **/

#ifndef BLYNKMANAGER_H
#define BLYNKMANAGER_H


#include <Arduino.h>
#include <BlynkSimpleEsp8266_SSL.h>

// blynk connection check interval in seconds
const uint8_t BLYNK_CHECK_INTERVAL = 30;

class BlynkManager {
    public:
        static void init(uint8_t pin);

        static void sendTempToBlynk(float temp, uint8_t blynkPin);

        static void sendRhToBlynk(float rh, uint8_t blynkPin);

        static void ensureBlynkConnection();

};

#endif