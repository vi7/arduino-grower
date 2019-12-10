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
#include "water_schedule.h"

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
#define PUMPPIN D2
#define LDRPIN A0

/* Blynk defines */
#define BLYNK_LCDPIN V0
#define BLYNK_GRAPHTEMPPIN V1
#define BLYNK_GRAPHRHPIN V2
#define BLYNK_LAMPBRLEDPIN V4
#define BLYNK_LAMPLEDPIN V5
#define BLYNK_GRAPHPUMPPIN V6
#define BLYNK_FANLEDPIN V7
#define BLYNK_HUMLEDPIN V8

/* normally closed (NC) relay NC defines */
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define PUMP_ON HIGH
#define PUMP_OFF LOW

/* temperature monitoring constants */
const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 10;

/* light monitoring constants */
// lamp check interval in seconds
const uint8_t LIGHT_CHECK_INTERVAL = 1;
// LDR sensor values. The lower value is the brighter is light.
const uint8_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

const String location = "Europe/Amsterdam";

SimpleTimer timer;
DHTesp dht;
float temp, rH;
uint16_t dhtReadInterval;
WiFiServer server(80);
String request;
bool isLampPowerOn, isLampAutoPowerOn, isLampOn;
bool isFanPowerOn;
bool isHumPowerOn;
Timezone LocalTZ;
Scheduler waterScheduler(water, WATER_SCHEDULE.sec, WATER_SCHEDULE.min, WATER_SCHEDULE.hr, WATER_SCHEDULE.day, WATER_SCHEDULE.mnth, WATER_SCHEDULE.year, WATER_SCHEDULE.intervalDays, &LocalTZ);

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
  initEZT();
  initBlynk();
  initDHT();
  initLamp();

  // delay Blynk dependant init functions
  timer.setTimeout(2000, initLampRelay);
  timer.setTimeout(3000, initFanRelay);
  timer.setTimeout(4000, initHumRelay);
  timer.setTimeout(5000, initPump);

  /* SimpleTimer function execution scheduling */
  timer.setInterval(dhtReadInterval, tempRhDataHandler);
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, lampStatus);
  timer.setInterval(BLYNK_CHECK_INTERVAL * 1000, ensureBlynkConnection);

  /* ezTime function execution scheduling */
  LocalTZ.setEvent(waterScheduler.function, waterScheduler.getNextUnixTime());
  // TODO: candidate for debug logging
  // Serial.print(F("[MAIN] [D] Requested watering start time: "));
  // Serial.println(waterScheduler.getStartDateTime());
  // Serial.print(F("[MAIN] [D] Next watering scheduled on: "));
  // Serial.println(waterScheduler.getNextDateTime());

  server.begin();

}


/****************/
/*     LOOP     */
/****************/
void loop() {

  Blynk.run();
  timer.run();
  ezt::events();

  // TODO: move to a separate func or consider "`ESP8266WebServer.h`" usage
  WiFiClient client = server.available();

  if (client) {
      Serial.println("New Client.");

      String currentLine = "";
      while (client.connected()) {
          if (client.available()) {
              char c = client.read();
              Serial.write(c);
              request += c;
              if (c == '\n') {
                  if (currentLine.length() == 0) {
                    sendResponse(client);
                    break;
                  } else {
                  currentLine = "";
                  }
              } else if (c != '\r') {
              currentLine += c;
              }
          }
      }

      request = "";
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
  }
}


/***************************/
/*        FUNCTIONS        */
/***************************/
// TODO: consider "`ESP8266WebServer.h`" usage
void sendResponse(WiFiClient client) {
  Serial.println("Sending response");
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();

  /*
   * APIv1 code
   */
  if (request.indexOf("GET /v1/dht/temperature") >= 0) {
      client.println("{\"temperature\":\"" + String(temp, 1) +"\"}");

  } else if (request.indexOf("GET /v1/dht/humidity") >= 0) {
      client.println("{\"humidity\":\"" + String(rH, 0) +"\"}");

  } else if (request.indexOf("GET /v1/relay/power/on") >= 0) {
      manualLampPowerOn();
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v1/relay/power/off") >= 0) {
      manualLampPowerOff();
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v1/relay/power/status") >= 0) {
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v1/lamp/status") >= 0) {
      client.println("{\"lamp\":\"" + String(isLampOn) +"\"}");

  } else if (request.indexOf("GET /v1/system/restart") >= 0) {
      client.println("{\"restart\":\"OK\"}");
      delay(500);
      systemRestart();

  /*
   * APIv2 code
   */
  } else if (request.indexOf("GET /v2/relay/lamp/power/on") >= 0) {
      manualLampPowerOn();
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/lamp/power/off") >= 0) {
      manualLampPowerOff();
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/lamp/power/status") >= 0) {
      client.println("{\"power\":\"" + String(isLampPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/fan/power/on") >= 0) {
      manualFanPower(true);
      client.println("{\"power\":\"" + String(isFanPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/fan/power/off") >= 0) {
      manualFanPower(false);
      client.println("{\"power\":\"" + String(isFanPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/fan/power/status") >= 0) {
      client.println("{\"power\":\"" + String(isFanPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/hum/power/on") >= 0) {
      manualHumPower(true);
      client.println("{\"power\":\"" + String(isHumPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/hum/power/off") >= 0) {
      manualHumPower(false);
      client.println("{\"power\":\"" + String(isHumPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/relay/hum/power/status") >= 0) {
      client.println("{\"power\":\"" + String(isHumPowerOn) +"\"}");

  } else if (request.indexOf("GET /v2/water/schedule/start") >= 0) {
      client.println("{\"date_time\":\"" + waterScheduler.getStartDateTime(ISO8601) +"\"}");

  } else if (request.indexOf("GET /v2/water/schedule/next") >= 0) {
      client.println("{\"date_time\":\"" + waterScheduler.getNextDateTime(ISO8601) +"\"}");

  } else if (request.indexOf("GET /v2/water/schedule/interval") >= 0) {
      client.println("{\"days\":\"" + String(WATER_SCHEDULE.intervalDays) +"\"}");

  } else if (request.indexOf("GET /v2/water/schedule/duration") >= 0) {
      client.println("{\"duration\":\"" + String(WATER_DURATION) +"\"}");

  }

  client.println();
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

  checkTemp();
  sendTempRhToBlynk();
}

void water() {
  pumpOn();
  timer.setTimeout(WATER_DURATION * 1000, pumpOff);
  LocalTZ.setEvent(waterScheduler.function, waterScheduler.getNextUnixTime());
  // TODO: candidate for debug logging
  // Serial.print(F("[MAIN] [DEBUG] Next watering re-scheduled on: "));
  // Serial.println(waterScheduler.getNextDateTime());
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

void initEZT() {
  // ezTime logging level, one of: NONE, ERROR, INFO, DEBUG
  ezt::setDebug(NONE);
  ezt::waitForSync();

  if (LocalTZ.setCache(0)) {
    Serial.println(F("Using cached timezone info"));
  } else {
    Serial.println(F("Timezone info not found in cache, fetching it"));
    LocalTZ.setLocation(location);
  }
	Serial.println(location + " time: " + LocalTZ.dateTime());
}

void initDHT() {
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.setup(DHTPIN, DHTesp::DHT22);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  Serial.println("DHT sensor read interval is: " + String(dhtReadInterval));
}

void initLampRelay() {
  pinMode(LAMPRELAYPIN, OUTPUT);
  isLampPowerOn = commonPower(LAMPRELAYPIN, true, &blynkLampLed);
  isLampAutoPowerOn = true;
}

void initFanRelay() {
  pinMode(FANRELAYPIN, OUTPUT);
  isFanPowerOn = commonPower(FANRELAYPIN, true, &blynkFanLed);
}

void initHumRelay() {
  pinMode(HUMRELAYPIN, OUTPUT);
  isHumPowerOn = commonPower(HUMRELAYPIN, true, &blynkHumLed);
}

void initPump() {
  pinMode(PUMPPIN, OUTPUT);
  digitalWrite(PUMPPIN, PUMP_OFF);

  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);
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

// function performs actions when temperature exceeds MAX_TEMP limit
void checkTemp() {
  if (!isLampAutoPowerOn) return;
  if (temp >= MAX_TEMP && isLampPowerOn) {
    isLampPowerOn = commonPower(LAMPRELAYPIN, false, &blynkLampLed);
  }
  else if (temp < MAX_TEMP - TEMP_HYSTERESIS && !isLampPowerOn) {
    isLampPowerOn = commonPower(LAMPRELAYPIN, true, &blynkLampLed);
  }
}

bool commonPower(uint8_t pin, bool enabled, WidgetLED *led) {
  bool isOn;
  if (enabled) {
    digitalWrite(pin, RELAY_ON);
    isOn = true;
    Serial.println("Blynk: enabling led for pin: " + String(pin));
    led->on();
  } else {
    digitalWrite(pin, RELAY_OFF);
    isOn = false;
    Serial.println("Blynk: disabling led for pin: " + String(pin));
    led->off();
  }
  return isOn;
}

void manualLampPowerOn() {
  isLampAutoPowerOn = true;
  isLampPowerOn = commonPower(LAMPRELAYPIN, true, &blynkLampLed);
}

void manualLampPowerOff() {
   isLampAutoPowerOn = false;
   isLampPowerOn = commonPower(LAMPRELAYPIN, false, &blynkLampLed);
}

void manualFanPower(bool enabled) {
  isFanPowerOn = commonPower(FANRELAYPIN, enabled, &blynkFanLed);
}

void manualHumPower(bool enabled) {
  isHumPowerOn = commonPower(HUMRELAYPIN, enabled, &blynkHumLed);
}

void pumpOn() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("[MAIN] [I] Pump is on"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 1);
}

void pumpOff() {
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("[MAIN] [I] Pump is off"));

  Serial.println(F("Blynk: sending pump status"));
  Blynk.virtualWrite(BLYNK_GRAPHPUMPPIN, 0);
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
