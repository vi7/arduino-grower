/**
 * WaterDevice.cpp
 **/

#include "WaterDevice.h"


WaterDevice::WaterDevice() {
};

void WaterDevice::init() {
  pinMode(PUMPPIN, OUTPUT);
  digitalWrite(PUMPPIN, PUMP_OFF);
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);

  waterScheduler.init(water, WATER_SCHEDULE);
}

void WaterDevice::init(uint8_t pin, uint8_t blynkPin) {
  this->pin = pin;
  this->blynkPin = blynkPin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, PUMP_OFF);
  Blynk.virtualWrite(blynkPin, 0);
};

void WaterDevice::powerOn() {
  // TODO implement after scheduler init will be moved to main
}


void WaterDevice::powerOff() {
  // TODO implement after scheduler init will be moved to main
}

String WaterDevice::status() {
  return "{\"date_time\":\"" + waterScheduler.getStartDateTime(ISO8601) +"\"," 
  + "\"date_time\":\"" + waterScheduler.getNextDateTime(ISO8601) + "\"," 
  + "\"days\":\"" + String(WATER_SCHEDULE.intervalDays) +"\","
  + "\"duration\":\"" + String(WATER_DURATION) +"\"}";
}

// TODO toBeRemoved after scheduler init will be moved to main
void WaterDevice::pumpOn() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("[MAIN] [I] Pump is on"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 1);
}

// TODO toBeRemoved after scheduler init will be moved to main
void WaterDevice::pumpOff() {
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("[MAIN] [I] Pump is off"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);
}

void WaterDevice::water() {
  pumpOn();
  timer.setTimeout(WATER_DURATION * 1000, pumpOff);
  waterScheduler.setNextEvent();
};