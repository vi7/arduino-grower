/**
 * WaterDevice.cpp
 **/

#include "WaterDevice.h"

void WaterDevice::init() {
  pinMode(PUMPPIN, OUTPUT);
  digitalWrite(PUMPPIN, PUMP_OFF);
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);

  scheduler.init(water, WATER_SCHEDULE);
};

void WaterDevice::water() {
  pumpOn();
  timer.setTimeout(WATER_DURATION * 1000, pumpOff);
  scheduler.setNextEvent();
};

void WaterDevice::pumpOn() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("[MAIN] [I] Pump is on"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 1);
}

void WaterDevice::pumpOff() {
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("[MAIN] [I] Pump is off"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);
}
