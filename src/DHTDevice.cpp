/**
 * DHTDevice.cpp
 **/

#include "DHTDevice.h"

void DHTDevice::init(uint8_t pin, uint8_t blynkTempPin) {
  pinMode(pin, INPUT_PULLUP);
  dht.setup(pin, DHT_MODEL);
  dhtReadInterval = (dht.getMinimumSamplingPeriod());
  Serial.printf((char*)F("DHT sensor read interval is: %u\n"), dhtReadInterval);
}

void DHTDevice::init(uint8_t pin, uint8_t blynkTempPin, uint8_t blynkRhPin) {
  init(pin, blynkTempPin);
  this->blynkTempPin = blynkTempPin;
  this->blynkRhPin = blynkRhPin;
};

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
void DHTDevice::tempDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS) {
  float currTemp;
  currTemp = dht.getTemperature();
  if (isnan(currTemp)) {
    _tempReadErrs++;
    Serial.printf((char*)F("DHT: Failed to read temperature!\n\tcurrent value:\t\t\t%f\n\tlast valid value:\t\t%f\n"), currTemp, temp);
    return;
  }
  temp = currTemp;
  PowerManager::autoPower(&device->isAutoPowerOn, &device->isPowerOn, &temp, MAX_TEMP, TEMP_HYSTERESIS, device->pin, device->led);
  // BlynkManager::sendTempToBlynk(temp, blynkTempPin);
}

void DHTDevice::rhDataHandler(Device* device, uint8_t MAX_RH, uint8_t RH_HYSTERESIS) {
  float currRH;
  currRH = dht.getHumidity();
  if (isnan(currRH)) {
    _rHReadErrs++;
    Serial.printf((char*)F("DHT: Failed to read humidity!\n\tcurrent value:\t\t\t%f\n\tlast valid value:\t\t%f\n"), currRH, rH);
    return;
  }
  rH = currRH;
  PowerManager::autoPower(&device->isAutoPowerOn, &device->isPowerOn, &rH, MAX_RH, RH_HYSTERESIS, device->pin, device->led);
  // BlynkManager::sendRhToBlynk(rH, blynkRhPin);
}

void DHTDevice::powerOn() {

}

void DHTDevice::powerOff() {

}

String DHTDevice::status() {
    return "{\"temperature\":\"" + String(temp, 1) +"\",\"humidity\":\"" + String(rH, 0) +"\"}";
}


/*******************************/
/** Metrics collector methods **/
/******************************/

// TODO: move metrics methods to a standalone DHTDeviceCollector class
// Follow Prometheus libs and exporters development recommendations

void DHTDevice::collect(String *metrics) {
  _metrics.clear();

  metricTemp();
  metricHumidity();
  metricHeatIndex();
  metricPerception();
  metricTempReadErr();
  metricHumidityReadErr();

  *metrics = _metrics;
}

void DHTDevice::metricTemp() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_temp_celsius Air temperature.\n"
    "# TYPE dht_temp_celsius gauge\n"
    "dht_temp_celsius{model=\"%s\",pin=\"%d\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), temp);
  _metrics.concat(metric);
}

void DHTDevice::metricHumidity() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_humidity_percent Air relative humidity.\n"
    "# TYPE dht_humidity_percent gauge\n"
    "dht_humidity_percent{model=\"%s\",pin=\"%d\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), rH);
  _metrics.concat(metric);
}

void DHTDevice::metricHeatIndex() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_heat_index_celsius Apparent air temperature (how hot it really feels when relative humidity is factored in with the actual air temperature).\n"
    "# TYPE dht_heat_index_celsius gauge\n"
    "dht_heat_index_celsius{model=\"%s\",pin=\"%d\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), dht.computeHeatIndex(temp, rH, false));
  _metrics.concat(metric);
}

void DHTDevice::metricPerception() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_perception_index Human perception index. See https://github.com/beegee-tokyo/DHTesp README for the details.\n"
    "# TYPE dht_perception_index gauge\n"
    "dht_perception_index{model=\"%s\",pin=\"%d\"} %d\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), dht.computePerception(temp, rH, false));
  _metrics.concat(metric);
}

void DHTDevice::metricTempReadErr() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_temp_read_errors_total Total number of temperature read errors.\n"
    "# TYPE dht_temp_read_errors_total counter\n"
    "dht_temp_read_errors_total{model=\"%s\",pin=\"%d\"} %u\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), getTempReadErrs());
  _metrics.concat(metric);
}

void DHTDevice::metricHumidityReadErr() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP dht_humidity_read_errors_total Total number of humidity read errors.\n"
    "# TYPE dht_humidity_read_errors_total counter\n"
    "dht_humidity_read_errors_total{model=\"%s\",pin=\"%d\"} %u\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, DHT_NAME, dht.getPin(), getRHReadErrs());
  _metrics.concat(metric);
}
