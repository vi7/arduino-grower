/* temp-rh-pwr-control.ino */

#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHTesp.h>

// Comment this out to disable Blynk prints and save space:
//#define BLYNK_PRINT Serial
#define DHTPIN D0
#define RELAYPIN D1
// relay NC output is closed
#define RELAY_ON LOW
// relay NC output is opened
#define RELAY_OFF HIGH

////////////////////
// Blynk Settings //
////////////////////
//char BlynkAuth[] = "142da76c91af47b192d396xxxxxxxxx";
//char WiFiNetwork[] = "xxxxxxWlan";
//char WiFiPassword[] = "xxx_xxx_xxx_xxx";

const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 5;

SimpleTimer timer;
DHTesp dht;
float temp,rH;
long dhtReadInterval;

/*********/ 
/* SETUP */
/*********/
void setup() {
  
  // init Serial for logging and say Hi
  Serial.begin(115200);
  const String boardIdentity = ARDUINO_BOARD;
  Serial.print("\n\n" + boardIdentity);
  Serial.println(F(" is up. Hey there!"));
  
  initDHT();
  initRelay();
  
  // schedule dataHandler() func execution
  timer.setInterval(dhtReadInterval, dataHandler);

//    // Initialize Blynk
//    Blynk.begin(BlynkAuth, WiFiNetwork, WiFiPassword);

}

/********/ 
/* LOOP */
/********/
void loop() {
  
//    Blynk.run();

  timer.run();
}

/*************/ 
/* FUNCTIONS */
/*************/ 
void dataHandler() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  getTempRh();
  
  // TODO: implement the ability to swith logging on and off
  Serial.print(dht.getStatusString());
  Serial.print(F("\t"));
  Serial.print(F("temperature: "));
  Serial.print(temp);
  Serial.print(F("\t\t"));
  Serial.print(F("RH: "));
  Serial.println(rH);
  
  checkTemp();

//  Blynk.virtualWrite(10, t); // virtual pin
//  Blynk.virtualWrite(11, h); // virtual pin

}

void initDHT() {
  dht.setup(DHTPIN, DHTesp::DHT22);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
}

void initRelay() {
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, RELAY_ON);
}

void getTempRh() {
  rH = dht.getHumidity();
  temp = dht.getTemperature();
  if (isnan(rH) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void checkTemp() {
  if (temp >= MAX_TEMP) {
    powerOff(true);
  }
  else if (temp < MAX_TEMP - TEMP_HYSTERESIS) {
    powerOff(false);
  }
}

void powerOff(bool isOff) {
  isOff ? digitalWrite(RELAYPIN, RELAY_OFF) : digitalWrite(RELAYPIN, RELAY_ON);
}
