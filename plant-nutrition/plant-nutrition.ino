/*
 * Simple plant watering solution
 * 
 * Amount of connected pumps: 3
 * Pump type: AD20P-1230C 
 * Pump specs:
 *  - 12VDC
 *  - Hmax 300cm
 *  - Qmax 240L/H(66ml/s)
 *  - Qreal 180L/H(50ml/s)
 */

// TODO: migrate to more advanced timer which allows functions with arguments and delayed execution
#include <SimpleTimer.h>

#define PUMP1PIN 2
#define PUMP2PIN 3
#define PUMP3PIN 4
#define LEDPIN   LED_BUILTIN

SimpleTimer timer;

// pump1 - est water amount: 500ml - time in seconds
uint8_t pump1WaterDuration = 9;
// pump2 - est water amount: 350ml - time in seconds
uint8_t pump2WaterDuration = 7;
// pump3 - est water amount: 350ml - time in seconds
uint8_t pump3WaterDuration = 7;

// water each 4 days - value in miliseconds
uint32_t waterInterval = 4L * 24L * 60L * 60L * 1000L;

// TODO: review if it's still needed after refactoring
uint8_t interPumpDelay = 1;

/*********/
/* SETUP */
/*********/
void setup() {

  // init Serial for logging and say Hi
  Serial.begin(115200);
  Serial.println("Arduino is up. Hey there!");

  initPumps();
  initLed();

  timer.setInterval(waterInterval, callWater);

}

/********/
/* LOOP */
/********/
void loop() {

  timer.run();
 
}

/*************/ 
/* FUNCTIONS */
/*************/ 
void initPumps() {
  pinMode(PUMP1PIN, OUTPUT);
  digitalWrite(PUMP1PIN, LOW);
  pinMode(PUMP2PIN, OUTPUT);
  digitalWrite(PUMP2PIN, LOW);
  pinMode(PUMP3PIN, OUTPUT);
  digitalWrite(PUMP3PIN, LOW);
}

void initLed() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void callWater() {
  water(PUMP1PIN, pump1WaterDuration);
  delay(interPumpDelay * 1000);
  water(PUMP2PIN, pump2WaterDuration);
  delay(interPumpDelay * 1000);
  water(PUMP3PIN, pump3WaterDuration);
}

void water(uint8_t pumpPin, uint8_t waterDuration) {
  pumpOn(pumpPin);
  delay(waterDuration * 1000);
  pumpOff(pumpPin);
}

void pumpOn(uint8_t pumpPin) {
  String log;
  
  digitalWrite(LEDPIN, HIGH);
  digitalWrite(pumpPin, HIGH);
  log = "Pump " + String(pumpPin - 1) + " is on";
  Serial.println(log);
}

void pumpOff(uint8_t pumpPin) {
  String log;
  
  digitalWrite(pumpPin, LOW);
  log = "Pump " + String(pumpPin - 1) + " is off";
  Serial.println(log);
  digitalWrite(LEDPIN, LOW);
}
