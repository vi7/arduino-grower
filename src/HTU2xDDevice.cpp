/**
 * HTU2xDDevice.cpp
 **/

#include "HTU2xDDevice.h"

void HTU2xDDevice::tempDataHandler(Device* device, uint8_t MAX_TEMP, uint8_t TEMP_HYSTERESIS) {
  temp = htu2xD->readTemperature();
  if (temp == HTU2XD_SHT2X_SI70XX_ERROR) {
    _tempReadErrs++;
    Serial.printf((char*)F("%s: Failed to read temperature! CRC8 or communication error occurred\n"), HTU2XD_NAME);
    /*
     * trying to recover and doing that only for temperature reading
     * by following HTU2xD_SHT2x_Si70xx lib example
     */
    htu2xD->softReset();                              //last chance to make it alive, all registers (except heater bit) will set to default
    htu2xD->setHeater(false);                         //true=heater on, false=heater off
    htu2xD->setResolution(HUMD_12BIT_TEMP_14BIT);     //humidity 12-bit, temperature 14-bit
    return;
  }
  PowerManager::autoPower(&device->isAutoPowerOn, &device->isPowerOn, &temp, MAX_TEMP, TEMP_HYSTERESIS, this->_pin);
  // TODO: add delay(500) as in the HTU2xD_SHT2x_Si70xx lib example?
}

void HTU2xDDevice::rhDataHandler(Device* device, uint8_t MAX_RH, uint8_t RH_HYSTERESIS) {
  rH = htu2xD->getCompensatedHumidity(temp);
  if (rH == HTU2XD_SHT2X_SI70XX_ERROR) {
    _rHReadErrs++;
    Serial.printf((char*)F("%s: Failed to read humidity! CRC8 or communication error occurred\n"), HTU2XD_NAME);
    return;
  }
  PowerManager::autoPower(&device->isAutoPowerOn, &device->isPowerOn, &rH, MAX_RH, RH_HYSTERESIS, this->_pin);
}

String HTU2xDDevice::status() {
    return "{\"temperature\":\"" + String(temp, 1) +"\",\"humidity\":\"" + String(rH, 0) +"\"}";
}


/*******************************/
/** Metrics collector methods **/
/******************************/

// TODO: move metrics methods to a standalone HTU2xDDeviceCollector class
// Follow Prometheus libs and exporters development recommendations

void HTU2xDDevice::collect(String *metrics) {
  _metrics.clear();

  metricTemp();
  metricHumidity();
  metricUncompHum();
  metricTempReadErr();
  metricHumidityReadErr();

  *metrics = _metrics;
}

void HTU2xDDevice::metricTemp() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP htu2xd_temp_celsius Air temperature.\n"
    "# TYPE htu2xd_temp_celsius gauge\n"
    "htu2xd_temp_celsius{model=\"%s\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, HTU2XD_NAME, temp);
  _metrics.concat(metric);
}

void HTU2xDDevice::metricHumidity() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP htu2xd_humidity_percent Air relative humidity.\n"
    "# TYPE htu2xd_humidity_percent gauge\n"
    "htu2xd_humidity_percent{model=\"%s\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, HTU2XD_NAME, rH);
  _metrics.concat(metric);
}

void HTU2xDDevice::metricUncompHum() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP htu2xd_uncomp_humidity_percent Air relative humidity uncompensated.\n"
    "# TYPE htu2xd_uncomp_humidity_percent gauge\n"
    "htu2xd_uncomp_humidity_percent{model=\"%s\"} %f\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, HTU2XD_NAME, htu2xD->readHumidity());
  _metrics.concat(metric);
}

void HTU2xDDevice::metricTempReadErr() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP htu2xd_temp_read_errors_total Total number of temperature read errors.\n"
    "# TYPE htu2xd_temp_read_errors_total counter\n"
    "htu2xd_temp_read_errors_total{model=\"%s\"} %u\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, HTU2XD_NAME, getTempReadErrs());
  _metrics.concat(metric);
}

void HTU2xDDevice::metricHumidityReadErr() {
  static size_t const BUFSIZE = 256;
  static char const *responseTemplate =
    "# HELP htu2xd_humidity_read_errors_total Total number of humidity read errors.\n"
    "# TYPE htu2xd_humidity_read_errors_total counter\n"
    "htu2xd_humidity_read_errors_total{model=\"%s\"} %u\n";

  char metric[BUFSIZE];
  snprintf(metric, BUFSIZE, responseTemplate, HTU2XD_NAME, getRHReadErrs());
  _metrics.concat(metric);
}
