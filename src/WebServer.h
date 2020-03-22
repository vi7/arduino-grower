/**
 * WebServer.h
 **/


#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESP8266WebServer.h>
#include "Device.h"
#include "Types.h"

class WebServer: public ESP8266WebServer {
    public: 
        using ESP8266WebServer::ESP8266WebServer;

        // TODO possibility to pass array of endpoints
        void registerEndpoint(Device *device, endpoint endpoint);

        void sendReply(const String& content);
};

#endif