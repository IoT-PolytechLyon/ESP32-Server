#include "HttpServer.h"

HttpServer::HttpServer(int timeoutTime)
{
  this->_timeoutTime = timeoutTime;
}

void HttpServer::vConnectToWiFi()
{
  WiFi.begin(this->_ssid, this->_password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("connecting to WiFi...");
  }
  Serial.print("\nConnected to network : ");
  Serial.print(this->_ssid);
  Serial.print("\n");
  Serial.println(WiFi.localIP());
}


void HttpServer::vListenToIncomingConnections(WiFiServer& server)
{
  String httpRequest; 
  WiFiClient client = server.available(); // waits for incoming clients. This method is a blocking operation.
  // if we have a client requesting for something...
  if (client)
  {
    // and while he's connected & available, we read data.
    while(client.connected())
    {
      if(client.available())
      {
        while(client.available())
        {
          char c = client.read();
          httpRequest += c;
        }
        break; // once we reach this line, it means that all the data were read. We break from the while.connected() loop and reach the end of our method.
      }
    }
    // everything is read. We proceed to the request and we respond.
    client.println(this->handleEndpoints(httpRequest));
    client.println();
    client.println();
    client.stop();
  }
}

// MODIFY THIS METHOD TO ADD NEW ENDPOINTS
String HttpServer::handleEndpoints(String httpRequest)
{
  /********************** GET /built-in/on **********************/
  // this endpoint is for testing purposes. It turns the built-in led on.
  if (httpRequest.indexOf("GET /built-in/on") >= 0)
  {
    Serial.println("GET /built-in/on");
    digitalWrite(13, HIGH);
    return this->get200() + "{ \"data\": {\"success\": \"BUILT_IN LED was successfuly updated.\"} }";
  }
  /********************** GET /built-in/off **********************/
  // this endpoint is for testing purposes. It turns the built-in led off.
  else if (httpRequest.indexOf("GET /built-in/off") >= 0)
  {
    Serial.println("GET /built-in/off");
    digitalWrite(13, LOW);
    return this->get200() + "{ \"data\": {\"success\": \"BUILT_IN LED was successfuly updated.\"} }";
  }
  /********************** POST /led ****************************/
  // this is our main endpoint. It receives a json object that will change our led state.
  else if (httpRequest.indexOf("POST /led") >= 0)
  {
    Serial.println("POST /led");
    //Serial.println(httpRequest);
    String httpBody = HttpServer::getHttpBodyFromHttpRequest(httpRequest);
    //todo : retrieve the body, parse int, and proceed if valid.
    return this->get200() + "{ \"data\": {\"success\": \"POST /led is available but the endpoint does nothing for now.\"} }";
  }
  /********************** 404 **********************/
  else
  {
    return this->get404() + "{ \"data\": {\"error\": \"Unknown endpoint.\"} }";
  }
}

String HttpServer::get200()
{
  return "HTTP/1.1 200 OK\nContent-type:application/json\nConnection: close\n\n";
}

String HttpServer::get404()
{
  return "HTTP/1.1 404 OK\nContent-type:application/json\nConnection: close\n\n";
}

String HttpServer::getHttpBodyFromHttpRequest(String httpRequest)
{
  // returns everything that is located between the end of the header and the end of the request (i.e. the request body).
  return httpRequest.substring(httpRequest.indexOf("\r\n\r\n"), httpRequest.length());
}
