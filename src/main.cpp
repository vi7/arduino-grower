/*
 * main.cpp
 *
 * !!! IMPORTANT !!! don't send more that 10 values per second to Blynk
 *
 */

/* Comment this out to disable Blynk prints and save space: */
//#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints

#include "main.h"
#include "schedules.h"

/*
 * include secrets and credentials
 * file should be created manually on a basis of "secrets.h.example"
 * it'll be ignored by git
 */
#include "secrets.h"

#define DHTPIN D3
#define LAMPRELAYPIN D1
#define FANRELAYPIN D0
#define HUMRELAYPIN D4
#define LDRPIN A0

/* Blynk defines */
#define BLYNK_LCDPIN V0
#define BLYNK_GRAPHTEMPPIN V1
#define BLYNK_GRAPHRHPIN V2
#define BLYNK_LAMPBRLEDPIN V4
#define BLYNK_LAMPLEDPIN V5
#define BLYNK_FANLEDPIN V7
#define BLYNK_HUMLEDPIN V8

BlynkWifi<BlynkArduinoClientSecure<WiFiClientSecure> > Blynk(_blynkTransport);

// /* normally closed (NC) relay NC defines */

/* monitoring constants */
const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 10;
// Recommended RH values:
// - vegetative - 60%
// - flowering - 50%
const uint8_t MAX_RH = 50;
const uint8_t RH_HYSTERESIS = 10;
// lamp check interval in seconds
const uint8_t LIGHT_CHECK_INTERVAL = 1;
// LDR sensor values. The lower value is the brighter is light.
const uint8_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

SimpleTimer timer;
DHTesp dht;
float temp, rH;
uint16_t dhtReadInterval;
ESP8266WebServer server(80);
String request;
bool isLampPowerOn, isLampAutoPowerOn, isLampOn;
bool isFanPowerOn;
bool isHumPowerOn, isHumAutoPowerOn;

Device lamp;
Device fan;
Device hum;

Scheduler waterScheduler;
Scheduler lampOnScheduler;
Scheduler lampOffScheduler;
Scheduler fanOnScheduler;
Scheduler fanOffScheduler;
Scheduler humOnScheduler;
Scheduler humOffScheduler;

/* Blynk stuff */
// blynk connection check interval in seconds
const uint8_t BLYNK_CHECK_INTERVAL = 30;

WidgetLCD blynkLcd(BLYNK_LCDPIN);
WidgetLED blynkLampBrLed(BLYNK_LAMPBRLEDPIN);
WidgetLED blynkLampLed(BLYNK_LAMPLEDPIN);
WidgetLED blynkFanLed(BLYNK_FANLEDPIN);
WidgetLED blynkHumLed(BLYNK_HUMLEDPIN);


/*****************/
/*     SETUP     */
/*****************/
void setup() {

  // init Serial for logging and say Hi
  Serial.begin(115200);
  const char BOARD_IDENTITY[] = ARDUINO_BOARD;
  Serial.println(F("\n\n************"));
  Serial.printf("%s", BOARD_IDENTITY);
  Serial.println(F(" is up. Hey there!"));
  Serial.printf("Firmware version: %s\n", GROWER_VERSION);
  Serial.println(F("************\n\n"));

  initWiFi(WIFI_SSID, WIFI_PSK);
  initBlynk();
  initDHT();
  initLamp();

  // delay Blynk dependant init functions
  timer.setTimeout(2000, []{lamp.init(LAMPRELAYPIN, &blynkLampLed);});
  timer.setTimeout(3000, []{fan.init(FANRELAYPIN, &blynkFanLed);});
  timer.setTimeout(4000, []{hum.init(HUMRELAYPIN, &blynkHumLed);});
  timer.setTimeout(5000, initPump);

  /* SimpleTimer function execution scheduling */
  timer.setInterval(dhtReadInterval, tempRhDataHandler);
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, lampStatus);
  timer.setInterval(BLYNK_CHECK_INTERVAL * 1000, ensureBlynkConnection);

  WaterDevice::init();

  lampOnScheduler.init([]{
        PowerManager::manualPower(true, LAMPRELAYPIN, &blynkLampLed, &isLampPowerOn, &isLampAutoPowerOn);
        lampOnScheduler.setNextEvent();
        }, LAMP_ON_SCHEDULE);
  lampOffScheduler.init(scheduledLampPowerOff, LAMP_OFF_SCHEDULE);
  fanOnScheduler.init(scheduledFanPowerOn, FAN_ON_SCHEDULE);
  fanOffScheduler.init(scheduledFanPowerOff, FAN_OFF_SCHEDULE);
  humOnScheduler.init(scheduledHumPowerOn, HUM_ON_SCHEDULE);
  humOffScheduler.init(scheduledHumPowerOff, HUM_OFF_SCHEDULE);

  // TODO: candidate for debug logging
  // Serial.print(F("[MAIN] [D] Requested watering start time: "));
  // Serial.println(waterScheduler.getStartDateTime());
  // Serial.print(F("[MAIN] [D] Next watering scheduled on: "));
  // Serial.println(waterScheduler.getNextDateTime());

  initServer();
  server.begin();

}


/****************/
/*     LOOP     */
/****************/
void loop() {

  Blynk.run();
  timer.run();
  ezt::events();
  server.handleClient();
}


/***************************/
/*        FUNCTIONS        */
/***************************/
void initServer() {
    server.on("/v2/dht/temperature", []{
    sendReply("{\"temperature\":\"" + String(temp, 1) +"\"}");
  });

  server.on("/v2/dht/humidity", [] {
    sendReply("{\"humidity\":\"" + String(rH, 0) +"\"}");
  });

  server.on("/v2/relay/lamp/power/on", []{
      PowerManager::manualPower(true, LAMPRELAYPIN, &blynkLampLed, &isLampPowerOn, &isLampAutoPowerOn);
      sendReply("{\"power\":\"" + String(isLampPowerOn) +"\"}");
  });

  server.on("/v2/relay/lamp/power/off", []{
      PowerManager::manualPower(false, LAMPRELAYPIN, &blynkLampLed, &isLampPowerOn, &isLampAutoPowerOn);
      sendReply("{\"power\":\"" + String(isLampPowerOn) +"\"}");
  });

  server.on("/v2/relay/lamp/power/status", []{
      sendReply("{\"power\":\"" + String(isLampPowerOn) +"\"}");    
  });

  server.on("/v2/relay/fan/power/on", [] {
      PowerManager::manualPower(true, FANRELAYPIN, &blynkFanLed, &isFanPowerOn);
      sendReply("{\"power\":\"" + String(isFanPowerOn) +"\"}");
  });

  server.on("/v2/relay/fan/power/off", [] {
      PowerManager::manualPower(false, FANRELAYPIN, &blynkFanLed, &isFanPowerOn);
      sendReply("{\"power\":\"" + String(isFanPowerOn) +"\"}");
  });

  server.on("/v2/relay/fan/power/status", [] {
      sendReply("{\"power\":\"" + String(isFanPowerOn) +"\"}");
  });

  server.on("/v2/relay/hum/power/on", [] {
      PowerManager::manualPower(true, HUMRELAYPIN, &blynkHumLed, &isHumPowerOn, &isHumAutoPowerOn);
      sendReply("{\"power\":\"" + String(isHumPowerOn) +"\"}");
  });

  server.on("/v2/relay/hum/power/off", [] {
      PowerManager::manualPower(false, HUMRELAYPIN, &blynkHumLed, &isHumPowerOn, &isHumAutoPowerOn);
      sendReply("{\"power\":\"" + String(isHumPowerOn) +"\"}");
  });

  server.on("/v2/relay/hum/power/status", [] {
      sendReply("{\"power\":\"" + String(isHumPowerOn) +"\"}");
  });

  server.on("/v2/water/schedule/start", [] {
      sendReply("{\"date_time\":\"" + waterScheduler.getStartDateTime(ISO8601) +"\"}");
  });

  server.on("/v2/water/schedule/next", [] {
      sendReply("{\"date_time\":\"" + waterScheduler.getNextDateTime(ISO8601) +"\"}");
  });

  server.on("/v2/water/schedule/interval", [] {
      sendReply("{\"days\":\"" + String(WATER_SCHEDULE.intervalDays) +"\"}");
  });

  server.on("/v2/water/schedule/duration", [] {
      sendReply("{\"duration\":\"" + String(WATER_DURATION) +"\"}");
  });
}

void sendReply(const String& content) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", content);
}

// function gets and uses temperature and relative humidity (RH) data
void tempRhDataHandler() {
  getTempRh();

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

  // automatic temperature monitoring
  PowerManager::autoPower(&isLampAutoPowerOn, &isLampPowerOn, &temp, MAX_TEMP, TEMP_HYSTERESIS, LAMPRELAYPIN, &blynkLampLed);

  // automatic relative humidity monitoring
  PowerManager::autoPower(&isHumAutoPowerOn, &isHumPowerOn, &rH, MAX_RH, RH_HYSTERESIS, HUMRELAYPIN, &blynkHumLed);

  sendTempRhToBlynk();
}


//  TODO: refactor this function to be similar to tempRhDataHandler()
void lampStatus() {
  uint16_t lightVal = getLightValue();
  uint8_t ledBrightness = map(lightVal, 0, 1023, 255, 0);
  // TODO: candidate for debug logging
  // Serial.println("LDR sensor: " + String(lightVal));
  if (lightVal < LAMP_ON_VALUE && !isLampOn) {
    Serial.println(F("LDR sensor: lamp is on"));
    isLampOn = true;
  } else if (lightVal > LAMP_OFF_VALUE && isLampOn) {
    Serial.println(F("LDR sensor: lamp is off"));
    isLampOn = false;
  }
  sendLampToBlynk(ledBrightness);
}

// TODO: check if this really needed, assuming that Blynk connection still fails after WiFi issues
void ensureBlynkConnection() {
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


/**************************/
/*     INIT FUNCTIONS     */
/**************************/
void initWiFi(String SSID, String PSK) {
  WiFi.begin(SSID, PSK);

  Serial.print("Connecting to: " + SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void initBlynk() {
  Blynk.config(BLYNK_AUTH);
}

void initDHT() {
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.setup(DHTPIN, DHTesp::DHT22);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  Serial.println("DHT sensor read interval is: " + String(dhtReadInterval));
}

void initLamp() {
  uint16_t lightVal = getLightValue();
  isLampOn = lightVal < LAMP_ON_VALUE ? true : false;
}


/*****************************/
/*     UTILITY FUNCTIONS     */
/*****************************/
/* TODO: refactor getTempRh() func to use TempAndHumidity struct
 * from the DHT lib instead of separate temp and RH vars
 */
void getTempRh() {
  rH = dht.getHumidity();
  temp = dht.getTemperature();
  if (isnan(rH) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

uint16_t getLightValue() {
  return analogRead(LDRPIN);
}


void scheduledLampPowerOn() {
  PowerManager::manualPower(true, LAMPRELAYPIN, &blynkLampLed, &isLampPowerOn, &isLampAutoPowerOn);
  lampOnScheduler.setNextEvent();
}

void scheduledLampPowerOff() {
  PowerManager::manualPower(false, LAMPRELAYPIN, &blynkLampLed, &isLampPowerOn, &isLampAutoPowerOn);
  lampOffScheduler.setNextEvent();
}

void scheduledFanPowerOn() {
  PowerManager::manualPower(true, FANRELAYPIN, &blynkFanLed, &isFanPowerOn);
  fanOnScheduler.setNextEvent();
}

void scheduledFanPowerOff() {
  PowerManager::manualPower(false, FANRELAYPIN, &blynkFanLed, &isFanPowerOn);
  fanOffScheduler.setNextEvent();
}

void scheduledHumPowerOn() {
  PowerManager::manualPower(true, HUMRELAYPIN, &blynkHumLed, &isHumPowerOn, &isHumAutoPowerOn);
  humOnScheduler.setNextEvent();
}

void scheduledHumPowerOff() {
  PowerManager::manualPower(false, HUMRELAYPIN, &blynkHumLed, &isHumPowerOn, &isHumAutoPowerOn);
  humOffScheduler.setNextEvent();
}

void systemRestart() {
  Serial.println(F("Restarting device..."));
  ESP.restart();
}

void sendTempRhToBlynk() {

  // TODO: candidate for debug logging
  // Serial.println(F("Blynk: sending temperature and RH data"));

  // send data to the LCD widget
  String tempStr = "Temp: " + String(temp, 1) + "℃";
  String rHStr = "RH: " + String(rH, 0) + "%";
  blynkLcd.clear();
  blynkLcd.print(0, 0, tempStr); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  blynkLcd.print(0, 1, rHStr);
  // send data to the SuperChart widget
  Blynk.virtualWrite(BLYNK_GRAPHTEMPPIN, temp);
  Blynk.virtualWrite(BLYNK_GRAPHRHPIN, rH);
}

void sendLampToBlynk(uint8_t brightness) {

  // TODO: candidate for debug logging
  // Serial.println(F("Blynk: sending lamp brightness"));
  blynkLampBrLed.setValue(brightness);
}
