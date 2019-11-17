Temperature, relative humidity (RH) and power control
=====================================================

Wemos D1 mini based remote controller/monitor

## Requirements

**IDE and Libraries**

IDE: VSCode + [PlatformIO](http://platformio.org) IDE extension

- [ESP8266 board support](https://github.com/esp8266/Arduino#installing-with-boards-manager)
- [DHT sensor library for ESPx](https://github.com/beegee-tokyo/DHTesp)
- [Blynk lib](https://github.com/blynkkk/blynk-library/releases) for integration with [Blynk IoT](https://blynk.io)
- [SimpleTimer lib](https://github.com/schinken/SimpleTimer)

**Hardware**

- [Wemos D1 mini](https://wiki.wemos.cc/products:d1:d1_mini) or compatible clone
- [DHT22 (AM2302) sensor](https://learn.adafruit.com/dht)
- [Songle 230V relay switch](http://www.circuitbasics.com/wp-content/uploads/2015/11/5V-Relay-Pinout1.png)
- [Omron 230V 2 channel solid state relay](https://hobbycomponents.com/2362-thickbox_default/5v-2-channel-240v-ac-solid-state-relay.jpg)

## Hardware configuration

Drawn by hand schematic is [here](resources/arduino-grower-schematic-by-hand.jpg). It'll be eventually replaced by something more professional in the future.

## Sofware configuration

[client](client) dir contains web client for the built-in web server and docker-compose for launching the nginx ready for serving this client.
