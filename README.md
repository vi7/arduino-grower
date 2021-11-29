[![CircleCI](https://circleci.com/gh/vi7/arduino-grower/tree/master.svg?style=svg)](https://circleci.com/gh/vi7/arduino-grower/tree/master)

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
- [HTU21D temp/humidity sensor](https://www.te.com/usa-en/product-CAT-HSC0004.html)
- [Songle 230V relay switch](http://www.circuitbasics.com/wp-content/uploads/2015/11/5V-Relay-Pinout1.png)
- [Omron 230V 2 channel solid state relay](https://hobbycomponents.com/2362-thickbox_default/5v-2-channel-240v-ac-solid-state-relay.jpg)
- LDR sensor
- 12V water pump (optional)

Hardware configuration
----------------------

Drawn by hand schematic is [here](resources/arduino-grower-schematic-by-hand.jpg). It'll be eventually replaced by something more professional in the future.

Sofware configuration
---------------------

### Web client

Deploy web client container to the host defined in the [.circleci/ci.env](.circleci/ci.env):
```bash
make client-deploy
```

Version of the client's Docker image will be by default taken from [include/version.h](include/version.h). To override the version:
```bash
make APP_VERSION=x.y.z client-deploy
```

### Monitoring

Prometheus metrics are available at the `/metrics` endpoint

Example Grafana dashboard: [resources/grafana_dashboard.json](resources/grafana_dashboard.json)

Development
-----------

### Web client

Test the web client by building and running its Docker image locally:
```bash
make client-test
```

> In case of successfull test container will be stopped and removed automatically

Build web client Docker image and run container locally without tests and automatic cleanup:
```bash
# After running 'make' web client will be available at http://localhost:8080
make client-run
```

To stop and remove local web client container:
```bash
make client-rm
```

### Release process

1. Set release version in the [include/version.h](include/version.h), commit and push to `master`, unless version bump was already delivered as a part of your changeset
2. Run `make release`, this will tag the latest `master` with the release version and push to the upstream and then trigger CI jobs with compilation and web client Docker image release
