/**
 * DHTDevice.cpp
 **/

#include "DHTDevice.h"

void DHTDevice::init(uint8_t pin, uint8_t blynkTempPin) {
// TODO 
  pinMode(pin, INPUT_PULLUP);
  dht.setup(pin, DHTesp::DHT22);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  Serial.println("DHT sensor read interval is: " + String(dhtReadInterval));
}

void DHTDevice::init(uint8_t pin, uint8_t blynkTempPin, uint8_t blynkRhPin) {
  init(pin, blynkTempPin);
  this->blynkTempPin = blynkTempPin;
  this->blynkRhPin = blynkRhPin;
};

  /************ LOGGING ************/
  // TODO: candidate for debug logging
  // Serial.print("DHT " + String(dht.getStatusString()));
  // Serial.print(F("\t"));
  // Serial.print(F("temperature: "));
  // Serial.print(temp);
  // Serial.print(F("\t\t"));
  // Serial.print(F("RH: "));
  // Serial.println(rH);
  /************  END LOGGING ************/
void DHTDevice::tempDataHandler(Device device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS) {
  temp = dht.getTemperature();
  if (isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

// TODO IMPORTANT! double test isPowerOn value in device after function execution
  PowerManager::autoPower(&device.isAutoPowerOn, &device.isPowerOn, &temp, MAX_TEMP, TEMP_HYSTERESIS, device.pin, device.led);
  BlynkManager::sendTempToBlynk(temp, blynkTempPin);
}

void DHTDevice::rhDataHandler(Device device, uint8_t MAX_RH, uint8_t RH_HYSTERESIS) {
  rH = dht.getHumidity();
  if (isnan(rH)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

// TODO IMPORTANT! double test isPowerOn value in device after function execution
  PowerManager::autoPower(&device.isAutoPowerOn, &device.isPowerOn, &rH, MAX_RH, RH_HYSTERESIS, device.pin, device.led);
  BlynkManager::sendRhToBlynk(rH, blynkRhPin);
}

void DHTDevice::powerOn() {

}

void DHTDevice::powerOff() {

}

String DHTDevice::status() {
    return "{\"temperature\":\"" + String(temp, 1) +"\",\"humidity\":\"" + String(rH, 0) +"\"}";
}