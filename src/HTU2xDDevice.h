/**
 * HTU2xDDevice.h
 **/

#ifndef HTU2XDDEVICE_H
#define HTU2XDDEVICE_H

#include <Arduino.h>
#include <HTU2xD_SHT2x_Si70xx.h>
#include "Device.h"
#include "MetricsCollectable.h"

#define HTU2XD_NAME "HTU21D"
#define HTU2XD_RESPONSE_TIME_MS 10000

class HTU2xDDevice: public Device, MetricsCollectable {

    public:
        float temp, rH;

        HTU2xDDevice(){
            htu2xD = new HTU2xD_SHT2x_SI70xx(HTU2xD_SENSOR, HUMD_12BIT_TEMP_14BIT);
            while (htu2xD->begin() != true) {
                Serial.println(F("HTU2xD/SHT2x not connected, fail or VDD < +2.25v"));
                delay(5000);
            }
            Serial.println(F("HTU2xD/SHT2x OK"));
        }

        void tempDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void rhDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void powerOn();

        void powerOff();

        String status();

        void collect(String *metrics);

        uint32_t getTempReadErrs() { return this->_tempReadErrs; }

        uint32_t getRHReadErrs() { return this->_rHReadErrs; }

        uint16_t getReadInterval() { return HTU2XD_RESPONSE_TIME_MS; }

    private:
        HTU2xD_SHT2x_SI70xx *htu2xD;
        String _metrics;
        uint32_t _tempReadErrs = 0, _rHReadErrs = 0;

        void metricTemp();
        void metricHumidity();
        void metricUncompHum();
        void metricTempReadErr();
        void metricHumidityReadErr();
};

#endif