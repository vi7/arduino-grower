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

// Comment this out to disable Blynk prints and save space:
//#define BLYNK_PRINT Serial
#define DHTPIN D0
#define RELAYPIN D1
#define PUMPPIN D2
#define PUMPPIN D2
#define LDRPIN A0
#define BLYNK_LCDPIN V0
#define BLYNK_GRAPHPIN1 V1
#define BLYNK_GRAPHPIN2 V2
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
const uint16_t LIGHT_CHECK_INTERVAL = 1;
const uint16_t LAMP_ON_VALUE = 200;
const uint16_t LAMP_OFF_VALUE = 900;

SimpleTimer timer;
DHTesp dht;
float temp,rH;
uint16_t dhtReadInterval;
WiFiServer server(80);
String request;
boolean isPowerOn;
boolean isAutoPowerOn;
boolean isLampOn;

/* Blynk stuff */
WidgetLCD lcd(BLYNK_LCDPIN);
WidgetLED lampBrLED(BLYNK_LAMPBRLEDPIN);
WidgetLED lampLED(BLYNK_LAMPLEDPIN);

/*********/ 
/* SETUP */
/*********/
void setup() {
  
  // init Serial for logging and say Hi
  Serial.begin(115200);
  const String boardIdentity = ARDUINO_BOARD;
  Serial.print("\n\n" + boardIdentity);
  Serial.println(F(" is up. Hey there!"));
  
  initRelay();
  initDHT();
  initBlynk();
  initPump();
  
  // schedule functions execution
  timer.setInterval(dhtReadInterval, tempRhDataHandler);
  timer.setInterval(WATER_INTERVAL, water);
  timer.setInterval(LIGHT_CHECK_INTERVAL * 1000, lampStatus);

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}

/********/
/* LOOP */
/********/
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

/*************/ 
/* FUNCTIONS */
/*************/ 
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
  sendToBlynk();
}

void water() {
  digitalWrite(PUMPPIN, PUMP_ON);
  Serial.println(F("Pump is on"));
  // TODO: do without a delay using some Timer solution, SimpleTimer doesn't work for that
  delay(WATER_DURATION * 1000);
  digitalWrite(PUMPPIN, PUMP_OFF);
  Serial.println(F("Pump is off"));
}

void lampStatus() {
  uint16_t lightVal = getLightValue();
  uint8_t ledBrightness = map(lightVal, 0, 1023, 255, 0);
  Serial.println("LDR sensor: " + String(lightVal));
  if (lightVal < LAMP_ON_VALUE && !isLampOn) {
    Serial.println(F("LDR sensor: lamp is on"));
    isLampOn = true;
    lampLED.on();
  } else if (lightVal > LAMP_OFF_VALUE && isLampOn) {
    Serial.println(F("LDR sensor: lamp is off"));
    isLampOn = false;
    lampLED.off();
  }
  lampBrLED.setValue(ledBrightness);
}

void initBlynk() {
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PSK);
}

void initDHT() {
  dht.setup(DHTPIN, DHTesp::DHT22);
//  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  /* Temporarily increased read interval to debug issues with the DHT sensor 
   *  
   *  TODO: revert back to the automatically obtained interval above (dht.getMinimumSamplingPeriod())
   *  after debugging is finished
   *  
   */
  dhtReadInterval = 4000;
  Serial.println("DHT sensor read interval is: " + String(dhtReadInterval));
}

void initRelay() {
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, RELAY_ON);
  isPowerOn = true;
  isAutoPowerOn = true;
}

void initPump() {
  pinMode(PUMPPIN, OUTPUT);
  digitalWrite(PUMPPIN, PUMP_OFF);
}

void getTempRh() {
  rH = dht.getHumidity();
  temp = dht.getTemperature();
  if (isnan(rH) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

uint32_t getLightValue() {
  return analogRead(LDRPIN);
}

// function performs actions when temperature exceeds MAX_TEMP limit
void checkTemp() {
  if (!isAutoPowerOn) return;
  if (temp >= MAX_TEMP) {
    powerOff(true);
  }
  else if (temp < MAX_TEMP - TEMP_HYSTERESIS) {
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

void sendToBlynk() {
  // send data to the LCD widget
  String tempStr = "Temp: " + String(temp, 1) + "℃";
  String rHStr = "RH: " + String(rH, 0) + "%";
  lcd.clear();
  lcd.print(0, 0, tempStr); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  lcd.print(0, 1, rHStr);
  // send data to the SuperChart widget
  Blynk.virtualWrite(BLYNK_GRAPHPIN1, temp);
  Blynk.virtualWrite(BLYNK_GRAPHPIN2, rH);
}

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
