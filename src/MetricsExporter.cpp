/**
 * MetricsExporter.cpp
 **/

#include "MetricsExporter.h"

void MetricsExporter::registerEndpoint() {
  _server->on(HTTP_METRICS_ENDPOINT, HTTPMethod::HTTP_GET, [this]() { this->handleMetrics(); });
}

void MetricsExporter::handleMetrics() {
  String metrics = String(' ');

  _device->collect(&metrics);
  _server->send(200, "text/plain; charset=utf-8", metrics);
}
