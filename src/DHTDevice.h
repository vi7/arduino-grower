/**
 * DHTDevice.h
 **/

#ifndef DHTDEVICE_H
#define DHTDEVICE_H

#include <Arduino.h>
#include <DHTesp.h>
#include "Device.h"
#include "MetricsCollectable.h"

#define DHT_MODEL DHTesp::DHT22
#define DHT_NAME "DHT22"

class DHTDevice: public Device, MetricsCollectable {

    public:
        uint16_t dhtReadInterval;
        float temp, rH;

        DHTDevice(uint8_t pin){
            this->_pin = pin;
            pinMode(pin, INPUT_PULLUP);
            dht.setup(pin, DHT_MODEL);
            dhtReadInterval = (dht.getMinimumSamplingPeriod());
            Serial.printf((char *)F("DHT sensor read interval is: %ums\n"), dhtReadInterval);
        }

        void tempDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void rhDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS);

        void powerOn();

        void powerOff();

        String status();

        void collect(String *metrics);

        uint32_t getTempReadErrs() { return this->_tempReadErrs; }

        uint32_t getRHReadErrs() { return this->_rHReadErrs; }

    private:
        DHTesp dht;
        String _metrics;
        uint32_t _tempReadErrs = 0, _rHReadErrs = 0;

        bool metricReady();
        void metricTemp();
        void metricHumidity();
        void metricHeatIndex();
        void metricPerception();
        void metricTempReadErr();
        void metricHumidityReadErr();
};

#endif