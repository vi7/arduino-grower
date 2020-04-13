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

        // Size of endpoints array passed explicitly because of array-to-pointer decay
        void registerEndpoint(Device *device, endpoint endpoints[], int size);

        void sendReply(const String& content);
};

#endif