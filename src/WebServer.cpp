/**
 *  WebServer.cpp
 */

#include "WebServer.h"

void WebServer::registerEndpoint(Device *device, endpoint endpoints[], int size) {
  for (int i = 0; i < size; i++) {
    endpoint endpoint = endpoints[i];
    switch (endpoint.type) {
    case POWER_ON:
      this->on(endpoint.url, [this, device] {
        device->powerOn();
        sendReply(device->status());
      });
      break;
    case POWER_OFF:
      this->on(endpoint.url, [this, device] {
        device->powerOff();
        sendReply(device->status());
      });
      break;
    case DEVICE_STATUS:
      this->on(endpoint.url, [this, device] {
        sendReply(device->status());
      });
      break;
    default:
      this->on(endpoint.url, [this, device] {
        sendReply(device->status());
      });
      break;
    }
  }
}

void WebServer::sendReply(const String &content)
{
  this->enableCORS(true);
  this->send(200, "application/json", content);
}
