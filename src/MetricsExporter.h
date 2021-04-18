/**
 * MetricsExporter.h
 **/

/**
 * Prometheus metrics exporter
 **/

#ifndef METRICSEXPORTER_H
#define METRICSEXPORTER_H

// Prometheus metrics endpoint
#define HTTP_METRICS_ENDPOINT "/metrics"

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "MetricsCollectable.h"

class MetricsExporter {

  public:
    MetricsExporter(ESP8266WebServer *server, MetricsCollectable *device)
    : _server(server)
    , _device(device)
    { this->registerEndpoint(); }

  private:
    ESP8266WebServer *_server;
    MetricsCollectable *_device;

    void registerEndpoint();
    void handleMetrics();
};

#endif
