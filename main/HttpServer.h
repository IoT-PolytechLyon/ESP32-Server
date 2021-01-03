#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include "Arduino.h"

class HttpServer
{
  private:
    // wifi credentials
    const char* _ssid = "D.";
    const char* _password = "p1805134@jessy63";
    int _timeoutTime;

    String handleEndpoints(String httpRequest);
    String get200();
    String get404();
    
  public:
    HttpServer(int timeoutTime);
    void vConnectToWiFi();
    void vListenToIncomingConnections(WiFiServer& server);
    static String getHttpBodyFromHttpRequest(String httpRequest);

};


#endif
