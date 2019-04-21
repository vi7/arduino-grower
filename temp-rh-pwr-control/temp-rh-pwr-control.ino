/* taken from https://escapequotes.net/esp8266-dht22-temperature-sensor/ */

// Comment this out to disable prints and save space:
//#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHTesp.h>

#define DHTPIN D0

////////////////////
// Blynk Settings //
////////////////////
//char BlynkAuth[] = "142da76c91af47b192d396xxxxxxxxx";
//char WiFiNetwork[] = "xxxxxxWlan";
//char WiFiPassword[] = "xxx_xxx_xxx_xxx";

SimpleTimer timer;
DHTesp dht;
float temp,rH;
long dhtReadInterval;

void setup() {
    Serial.begin(115200);
    String thisBoard = ARDUINO_BOARD;
    Serial.print("\n\n"+thisBoard);
    Serial.println(F(" is up. Hey there!"));

    dht.setup(DHTPIN, DHTesp::DHT22);
    dhtReadInterval = (dht.getMinimumSamplingPeriod());
    timer.setInterval(dhtReadInterval, sendData);

//    // Initialize Blynk
//    Blynk.begin(BlynkAuth, WiFiNetwork, WiFiPassword);

}

void loop() {
  
//    Blynk.run();

    timer.run();
}

void sendData()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  getTempRh();
  Serial.print(dht.getStatusString());
  Serial.print(F("\t"));
  Serial.print(F("temperature: "));
  Serial.print(temp);
  Serial.print(F("\t\t"));
  Serial.print(F("RH: "));
  Serial.println(rH);

//  Blynk.virtualWrite(10, t); // virtual pin
//  Blynk.virtualWrite(11, h); // virtual pin

}

void getTempRh() {
  rH = dht.getHumidity();
  temp = dht.getTemperature();
  if (isnan(rH) || isnan(temp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
  }
}
