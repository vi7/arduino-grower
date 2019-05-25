/*
 * temp-rh-pwr-control.ino
 * 
 * !!! IMPORTANT !!! don't send more that 10 values per second to Blynk
 * 
 * Watering solution:
 * Amount of connected pumps: 1
 * Pump type: AD20P-1230C 
 * Pump specs:
 *  - 12VDC
 *  - Hmax 300cm
 *  - Qmax 240L/H(66ml/s)
 *  - Qreal 180L/H(50ml/s)
 */

/* Comment this out to disable Blynk prints and save space: */
//#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <SimpleTimer.h>
#include <DHTesp.h>
/*
 * include secrets and credentials
 * file should be created manually on a basis of "secrets.h.example"
 * it'll be ignored by git
 */
#include "secrets.h"

#define DHTPIN D0
#define RELAYPIN D1
#define PUMPPIN D2
#define PUMPPIN D2
#define LDRPIN A0

/* Blynk defines */
#define BLYNK_LCDPIN V0
#define BLYNK_GRAPHTEMPPIN V1
#define BLYNK_GRAPHRHPIN V2
#define BLYNK_BUTTON1PIN V3
#define BLYNK_LAMPBRLEDPIN V4
#define BLYNK_LAMPLEDPIN V5

// relay NC output is closed
#define RELAY_ON LOW
// relay NC output is opened
#define RELAY_OFF HIGH
#define PUMP_ON HIGH
#define PUMP_OFF LOW

/* temperature monitoring constants */
const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 10;

/* watering constants */
// water each 5 days - time in milliseconds
const uint32_t WATER_INTERVAL = 5L * 24L * 60L * 60L * 1000L;
// est water amount: 500ml - time in seconds
const uint8_t WATER_DURATION = 9;

/* light monitoring constants */
// lamp check interval in seconds
const uint8_t LIGHT_CHECK_INTERVAL = 1;
// LDR sensor values. The lower value is the brighter is light.
const uint8_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

SimpleTimer timer;
DHTesp dht;
float temp,rH;
uint16_t dhtReadInterval;
WiFiServer server(80);
String request;
bool isPowerOn;
bool isAutoPowerOn;
bool isLampOn;

/* Blynk stuff */
// blynk connection check interval in seconds
const uint8_t BLYNK_CHECK_INTERVAL = 30;

WidgetLCD blynkLcd(BLYNK_LCDPIN);
WidgetLED blynkLampBrLed(BLYNK_LAMPBRLEDPIN);
WidgetLED blynkLampLed(BLYNK_LAMPLEDPIN);


/*****************/
/*     SETUP     */
/*****************/
void setup() {
  
  // init Serial for logging and say Hi
  Serial.begin(115200);
  const String boardIdentity = ARDUINO_BOARD;
  Serial.print("\n\n" + boardIdentity);
  Serial.println(F(" is up. Hey there!"));
  Serial.println("");

  initWiFi(WIFI_SSID, WIFI_PSK);
  initRelay();
  initDHT();
  initPump();
  initBlynk();
  
  // schedule functions execution
  timer.setInterval(dhtReadInterval, tempRhDataHandler);
  timer.setInterval(WATER_INTERVAL, water);
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, lampStatus);
  timer.setInterval(BLYNK_CHECK_INTERVAL * 1000, checkBlynkConnection);

  server.begin();

}


/****************/
/*     LOOP     */
/****************/
void loop() {
  
  Blynk.run();
  timer.run();

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
void sendResponse(WiFiClient client) {
  Serial.println("Sending response");
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();

  if (request.indexOf("GET /v1/dht/temperature") >= 0) {
      client.println("{\"temperature\":\"" + String(temp, 1) +"\"}");

  } else if (request.indexOf("GET /v1/dht/humidity") >= 0) {
      client.println("{\"humidity\":\"" + String(rH, 0) +"\"}");

  } else if (request.indexOf("GET /v1/relay/power/on") >= 0) {
      manualPowerOn();
      client.println("{\"power\":\"" + String(isPowerOn) +"\"}");
      
  } else if (request.indexOf("GET /v1/relay/power/off") >= 0) {
      manualPowerOff();
      client.println("{\"power\":\"" + String(isPowerOn) +"\"}");
     
  } else if (request.indexOf("GET /v1/relay/power/status") >= 0) {
      client.println("{\"power\":\"" + String(isPowerOn) +"\"}");
  
  } else if (request.indexOf("GET /v1/lamp/status") >= 0) {
      client.println("{\"lamp\":\"" + String(isLampOn) +"\"}");
  }
  client.println();
}

// function gets and uses temperature and relative humidity (RH) data
void tempRhDataHandler() {
  getTempRh();
  
  /************ LOGGING ************/
  // TODO: implement the ability to swith logging on and off
  Serial.print("DHT " + String(dht.getStatusString()));
  Serial.print(F("\t"));
  Serial.print(F("temperature: "));
  Serial.print(temp);
  Serial.print(F("\t\t"));
  Serial.print(F("RH: "));
  Serial.println(rH);
  /************  END LOGGING ************/
  
  checkTemp();
  sendTempRhToBlynk();
}

void water() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("Pump is on"));
  // TODO: do without a delay using some Timer solution, SimpleTimer doesn't work for that
  delay(WATER_DURATION * 1000);
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("Pump is off"));
}

//  TODO: refactor this function to be similar to tempRhDataHandler()
void lampStatus() {
  uint16_t lightVal = getLightValue();
  uint8_t ledBrightness = map(lightVal, 0, 1023, 255, 0);
  Serial.println("LDR sensor: " + String(lightVal));
  if (lightVal < LAMP_ON_VALUE && !isLampOn) {
    Serial.println(F("LDR sensor: lamp is on"));
    isLampOn = true;
  } else if (lightVal > LAMP_OFF_VALUE && isLampOn) {
    Serial.println(F("LDR sensor: lamp is off"));
    isLampOn = false;
  }
  sendLampToBlynk(isLampOn, ledBrightness);
}

void checkBlynkConnection() {
  if (Blynk.connected()) {
    Serial.println(F("Blynk watchdog: connected to the server"));
  } else {
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
  dht.setup(DHTPIN, DHTesp::DHT22);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  Serial.println("DHT sensor read interval is: " + String(dhtReadInterval));
}

void initRelay() {
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, RELAY_ON);
  isPowerOn = true;
  isAutoPowerOn = true;
  isLampOn = isPowerOn;
}

void initPump() {
  pinMode(PUMPPIN, OUTPUT);
  digitalWrite(PUMPPIN, PUMP_OFF);
}


/****************************/
/*     HELPER FUNCTIONS     */
/****************************/
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
  if (!isAutoPowerOn) return;
  if (temp >= MAX_TEMP && isPowerOn) {
    powerOff(true);
  }
  else if (temp < MAX_TEMP - TEMP_HYSTERESIS && !isPowerOn) {
    powerOff(false);
  }
}

void powerOff(bool yes) {
  if (yes) {
    digitalWrite(RELAYPIN, RELAY_OFF);
    isPowerOn = false;
  } else {
    digitalWrite(RELAYPIN, RELAY_ON);
    isPowerOn = true;
  }
  
  Serial.println(F("Blynk: sending power status"));
  Blynk.virtualWrite(BLYNK_BUTTON1PIN, isPowerOn);
}

void manualPowerOn() {
  isAutoPowerOn = true;
  powerOff(false);
}

void manualPowerOff() {
   isAutoPowerOn = false;
   powerOff(true);
}

void sendTempRhToBlynk() {
  
  Serial.println(F("Blynk: sending temperature and RH data"));
  
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

void sendLampToBlynk(bool isOn, uint8_t brightness) {

  Serial.println(F("Blynk: sending lamp status"));
  
  isOn ? blynkLampLed.on() : blynkLampLed.off();
  blynkLampBrLed.setValue(brightness);
}


/*****************/
/*     BLYNK     */
/*****************/
BLYNK_CONNECTED() {
  Blynk.virtualWrite(BLYNK_BUTTON1PIN, isPowerOn);
}

BLYNK_WRITE(BLYNK_BUTTON1PIN) {
  int buttonOn = param.asInt();
  if (buttonOn) {
      manualPowerOn();
    } else {
      manualPowerOff();    
    }
}
