#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include "ObjectData.h"

class HttpServer
{
  private:
    // wifi credentials
    const char*   _ssid = "Cocoon";
    const char*   _password = "myriam&dorian<3";
    
    const String  _nodeExpressServerIP = "192.168.0.55";
    const String  _nodeExpressServerPort = "8081";
    
    ObjectData*   _objectData;
    int           _timeoutTime;
                                                                        // Method order in .cpp for more convenience...
    
    String dispatcher(String httpRequest);                              // 6
    String get200();                                                    // 7
    String get404();                                                    // 8
    
    
  public:
    HttpServer(int timeoutTime);                                         // 1
    void vInitializeObjectData();                                        // 2
    ObjectData* getObjectState();                                        // 3
    void vConnectToWiFi();                                               // 4
    void vListenToIncomingConnections(WiFiServer& server);               // 5
    static String getHttpBodyFromHttpRequest(String httpRequest);        // 9
    void vPutStateToNodeExpressServer();                                 // 10
    ObjectData* getStateFromNodeExpressServer();                         // 11
    boolean isBadgeAuthorized(String badge);                             // 12
};


#endif
