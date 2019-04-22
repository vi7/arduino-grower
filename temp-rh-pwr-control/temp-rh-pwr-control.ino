/*
 * temp-rh-pwr-control.ino
 * 
 * !!! IMPORTANT !!! don't send more that 10 values per second to Blynk
 */

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <SimpleTimer.h>
#include <DHTesp.h>
/*
 * include secrets and credentials
 * file should be created manually and will be ignored by git
 */
#include "secrets.h"

// Comment this out to disable Blynk prints and save space:
#define BLYNK_PRINT Serial
#define DHTPIN D0
#define RELAYPIN D1
#define BLYNK_LCDPIN V0
// relay NC output is closed
#define RELAY_ON LOW
// relay NC output is opened
#define RELAY_OFF HIGH

const uint8_t MAX_TEMP = 40;
const uint8_t TEMP_HYSTERESIS = 5;

SimpleTimer timer;
DHTesp dht;
WidgetLCD lcd(BLYNK_LCDPIN);
float temp,rH;
long dhtReadInterval;
WiFiServer server(80);
String request;

/*********/ 
/* SETUP */
/*********/
void setup() {
  
  // init Serial for logging and say Hi
  Serial.begin(115200);
  const String boardIdentity = ARDUINO_BOARD;
  Serial.print("\n\n" + boardIdentity);
  Serial.println(F(" is up. Hey there!"));
  
  initBlynk();
  initDHT();
  initRelay();
  
  // schedule dataHandler() func execution
  timer.setInterval(dhtReadInterval, dataHandler);

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

                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-type:application/json");
                      client.println("Access-Control-Allow-Origin: *");
                      client.println("Connection: close");
                      client.println();

                      //todo implement with switch
                      if (request.indexOf("GET /v1/dht/temperature") >= 0) {
                          client.println("{\"temperature\":\"" + String(temp, 1) +"\"}");

                      } else if (request.indexOf("GET /v1/dht/humidity") >= 0) {
                          client.println("{\"humidity\":\"" + String(rH, 0) +"\"}");
                      }
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
  sendData();
}

void initBlynk() {
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PSK);
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

void sendData() {
  String tempStr = "Temp: " + String(temp, 1) + "℃";
  String rHStr = "RH: " + String(rH, 0) + "%";
  lcd.clear();
  lcd.print(0, 0, tempStr); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  lcd.print(0, 1, rHStr);
}
