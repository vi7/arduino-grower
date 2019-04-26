Environment manager for plant growing
=====================================


Temperature, relative humidity (RH) and power control
-----------------------------------------------------

## Requirements

**Libraries**

List of Arduino IDE libs

- [ESP8266 board support](https://github.com/esp8266/Arduino#installing-with-boards-manager)
- [DHT sensor library for ESPx](https://github.com/beegee-tokyo/DHTesp)
- [Blynk lib](https://github.com/blynkkk/blynk-library/releases) for integration with [Blynk IoT](https://blynk.io)
- [SimpleTimer lib](https://github.com/schinken/SimpleTimer)

**Hardware**

- [Wemos D1 mini](https://wiki.wemos.cc/products:d1:d1_mini) or compatible clone
- [DHT22 (AM2302) sensor](https://learn.adafruit.com/dht). See notes below
- 230V relay switch

# Setup

DHT22 used in the project has been already shipped with PCB and pullup resistor. [Click](resources/dht22-am2302-temperature-humidity.jpeg) to see the picture. Thus should be connected to the `D0` pin of the D1 mini board (no pullup)

*Schematics TBD*


Plant nutrition
---------------

Simple stupid watering solution. See further details in the project [README](plant-nutrition/README.md)