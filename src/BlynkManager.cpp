/**
 * BlynkManager.cpp
 **/

#include "BlynkManager.h"
#include "secrets.h"


BlynkWifi<BlynkArduinoClientSecure<WiFiClientSecure> > Blynk(_blynkTransport);
WidgetLCD blynkLcd(0);

void BlynkManager::init(uint8_t pin) {
    blynkLcd.setVPin(pin);
    Blynk.config(BLYNK_AUTH);
}

// TODO: candidate for debug logging
// Serial.println(F("Blynk: sending temperature and RH data"));
void BlynkManager::sendTempToBlynk(float temp, uint8_t blynkPin) {
  // send dta to the LCD widget
  String tempStr = "Temp: " + String(temp, 1) + "℃";
  blynkLcd.clear();
  blynkLcd.print(0, 0, tempStr); 
  // send data to the SuperChart widget
  Blynk.virtualWrite(blynkPin, temp);
}

void BlynkManager::sendRhToBlynk(float rh, uint8_t blynkPin) {
  String rHStr = "RH: " + String(rh, 0) + "%";
  blynkLcd.clear();
  blynkLcd.print(0, 1, rHStr);
  Blynk.virtualWrite(blynkPin, rh);
}

// TODO: check if this really needed, assuming that Blynk connection still fails after WiFi issues
void BlynkManager::ensureBlynkConnection() {
  if (Blynk.connected()) {
    // TODO: candidate for debug logging
    // Serial.println(F("Blynk watchdog: connected to the server"));
    return;
  } else {
    // TODO: candidate for error logging
    Serial.println(F("Blynk watchdog: connection to the server FAILED! Reconnecting..."));
    Blynk.disconnect();
    Blynk.connect(BLYNK_CHECK_INTERVAL);
  }
}


void BlynkManager::sendLampToBlynk(uint8_t brightness, WidgetLED *led) {

  // TODO: candidate for debug logging
  // Serial.println(F("Blynk: sending lamp brightness"));
  led->setValue(brightness);
}