IoT environment control station
===============================

Lolin (Wemos) D1 mini (ESP8266) environment control station.

Supports the following functionality:
- temperature and humidity monitoring
- light monitoring
- NTP time sync
- multiple power relay control
- automated actions based on the temperature and humidity
- automated actions based on schedules
- Prometheus compatible metrics
- REST API endpoints for the remote monitoring and control of the station

Requirements
------------

### IDE and Libraries

IDE: VSCode + [PlatformIO](http://platformio.org) IDE extension

Dependency libs are listed in the [platformio.ini](platformio.ini)

### Hardware

- [Wemos D1 mini](https://wiki.wemos.cc/products:d1:d1_mini) or compatible clone (not recommended, genuine device is always better)
- [DHT22 (AM2302) sensor](https://learn.adafruit.com/dht)
- [Songle 230V relay switch](http://www.circuitbasics.com/wp-content/uploads/2015/11/5V-Relay-Pinout1.png)
- [Omron 230V 2 channel solid state relay](https://hobbycomponents.com/2362-thickbox_default/5v-2-channel-240v-ac-solid-state-relay.jpg)
- LDR sensor
- 12V water pump (optional)

Hardware configuration
----------------------

Drawn by hand schematic is [here](resources/arduino-grower-schematic-by-hand.jpg). It'll be eventually replaced by something more professional in the future.

Sofware configuration
---------------------

[client](client) dir contains web client for the built-in web server and docker-compose for launching the nginx ready for serving this client.

Prometheus metrics are available at the `/metrics` endpoint
