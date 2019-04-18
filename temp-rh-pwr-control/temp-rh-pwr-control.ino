// taken from https://escapequotes.net/esp8266-dht22-temperature-sensor/

// TODO: refactor that

#define BLYNK_PRINT Serial // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>

#define DHTPIN D4 //pin gpio 2 in sensor

DHT dht(DHTPIN, DHT22);
//static const uint8_t SDA = 4;
//static const uint8_t SCL = 5;
//
//static const uint8_t LED_BUILTIN = 2;
//static const uint8_t BUILTIN_LED = 2;
//
//static const uint8_t D0 = 16;
//static const uint8_t D1 = 5;
//static const uint8_t D2 = 4;
//static const uint8_t D3 = 0;
//static const uint8_t D4 = 2;
//static const uint8_t D5 = 14;
//static const uint8_t D6 = 12;
//static const uint8_t D7 = 13;
//static const uint8_t D8 = 15;
//static const uint8_t RX = 3;
//static const uint8_t TX = 1;

////////////////////
// Blynk Settings //
////////////////////
char BlynkAuth[] = "142da76c91af47b192d396xxxxxxxxx";
char WiFiNetwork[] = "xxxxxxWlan";
char WiFiPassword[] = "xxx_xxx_xxx_xxx";
SimpleTimer timer;

float t,h;
unsigned long previousMillis = 0; // will store last temp was read
const long interval = 2000; // interval at which to read sensor

void setup() {
    Serial.begin(115200); // Serial
    // put your setup code here, to run once:
    pinMode(BUILTIN_LED, OUTPUT); // initialize onboard LED as output
    dht.begin();
    // Initialize Blynk
    Blynk.begin(BlynkAuth, WiFiNetwork, WiFiPassword);
    // Setup a function to be called 30 second
    timer.setInterval(30000L, sendUptime);
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(BUILTIN_LED, HIGH); // turn on LED with voltage HIGH
    delay(1000); // wait one second
    digitalWrite(BUILTIN_LED, LOW); // turn off LED with voltage LOW
    delay(1000); // wait one second
    Blynk.run();
    timer.run();
}

void sendUptime()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    //Read the Temp and Humidity from DHT

    gettemperature();
    Serial.print("temp:");
    Serial.println(t);
    Serial.print("humi:");
    Serial.println(h);
    Blynk.virtualWrite(10, t); // virtual pin
    Blynk.virtualWrite(11, h); // virtual pin
}

void gettemperature() {
    // Wait at least 2 seconds seconds between measurements.
    // if the difference between the current time and last time you read
    // the sensor is bigger than the interval you set, read the sensor
    // Works better than delay for things happening elsewhere also
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis >= interval) {
        // save the last time you read the sensor
        previousMillis = currentMillis;

        // Reading temperature for humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
        h = dht.readHumidity(); // Read humidity (percent)
        t = dht.readTemperature(); // Read temperature as C
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }
    }
}
