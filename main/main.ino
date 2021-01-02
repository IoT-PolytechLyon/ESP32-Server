/* libs */
#include "Arduino.h"
#include <NfcAdapter.h>
#include <PN532/PN532_HSU/PN532_HSU.h>
#include <WiFi.h>

/* consts */
#define NFC_READ_DELAY    500 //ms

/* global vars */
PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc(pn532hsu);
WiFiServer server(8080); // Server on port 8080. See https://randomnerdtutorials.com/esp32-web-server-arduino-ide/ for more details.
const long timeoutTime = 3000; // ms

// Change the wifi credentials here
const char* ssid = "D.";
const char* password = "p1805134@jessy63";

/********************************** NfcReader Class **********************************/
// TODO : SEE HOW IT CAN BE PUT INTO A SEPARATE FILE ?
// NB : NFC Reader must be plugged into UART port
class NfcReader
{
  private:
    int uxDelay;
    
  public:
    NfcReader(int uxDelayNewValue)
    {
      this->uxDelay = uxDelayNewValue;
      nfc.begin();
    }
    
    void vSetDelay(int uxDelayValue)
    {
      this->uxDelay = uxDelayValue;
    }

    void vReadNfc()
    {
      if (nfc.tagPresent())
      {
          NfcTag pvTag = nfc.read();
          pvTag.print();
      }
      delay(this->uxDelay);
    }
};
/********************************** WIFI conecction ******************************************/
void vConnectToWiFi()
{ 
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("connecting to WiFi...");
  }
  Serial.print("\nConnected to network : "); Serial.print(ssid); Serial.print("\n");
  Serial.println(WiFi.localIP());
}
/********************************** HTTP 8080 Server Task**************************************/
// this code was inspired by https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
void handleHttpConnections()
{
  unsigned long currentTime = 0;
  unsigned long previousTime = 0;   
  String httpRequest; // stores HTTP requests
  WiFiClient client = server.available(); // waits for incoming clients.
  if(client)
  {
    boolean ledUpdated = false;
    Serial.println("New client.\n");
    String clientData = "";
    currentTime = millis();
    previousTime = currentTime;
    
    while(client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();
      if(client.available()) // when there are data to read from the client
      {
        char c = client.read();
        httpRequest += c;
        if(c == '\n')
        {
          //end of the client request. We terminate the connexion. (\n + empty new line)
          if(clientData.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();

            // TODO : REFACTOR INTO A CLASS ?
            // THESE ARE THE AVAILABLE METHODS.
            if(httpRequest.indexOf("GET /built-in/on") >= 0)
            {
              Serial.println("GET /built-in/on");
              digitalWrite(13, HIGH);
              ledUpdated = true;
            }
            else if(httpRequest.indexOf("GET /built-in/off") >= 0)
            {
              Serial.println("GET /built-in/off");
              digitalWrite(13, LOW);
              ledUpdated = true;
            }
            //todo : other methods

            if(ledUpdated)
            {
              client.println("{ \"data\": {\"success\": \"LED was successfuly updated.\"} }");
            }
            else
            {
              client.println("{ \"data\": {\"error\": \"Unknown endpoint.\"} }");
            }

            client.println();
            break;
          }
          else
          {
            clientData = "";
          }
        }
        else if(c != '\r')
        {
          clientData += c;
        }
      }
    }

    httpRequest = "";
    client.stop();
    Serial.println("Client disconnected.\n");
  }
}
/********************************** HTTP Task ******************************************/
void vTaskHandleHttpConnections(void *pvParameters)
{
  server.begin();
  for(;;)
  {
    handleHttpConnections();
  }
}
/********************************** NFC Task *******************************************/
NfcReader reader = NfcReader(NFC_READ_DELAY);

void vTaskReadNfc(void *pvParameters)
{
  for(;;)
  {
    reader.vReadNfc();
  }
}
/********************************** Main *********************************************/
void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("--IoT--\n");
  pinMode(LED_BUILTIN, OUTPUT);
  //Connects to wifi network
  vConnectToWiFi();
  // JOB 1 : Starts NFC Reading.
  xTaskCreate(vTaskReadNfc, "vTaskReadNfc", 1000, NULL, 5, NULL);
  // JOB 2 : Starts the Http Server.
  xTaskCreate(vTaskHandleHttpConnections, "vTaskHandleHttpConnections", 8000, NULL, 7, NULL);
}

void loop() 
{
  //todo : si un badge est lu -> vérifier s'il correspond à un badge en BD. -> si oui, activer le PIR. PIR dans un JOB.
  //todo : si PIR detecte un mouvement -> regarder si un état est sauvegardé en BD -> si oui, l'utiliser pour allumer les LEDs, sinon, PUT un état par défaut (blanc) et l'utiliser (et allumer les leds).

  //todo : communiquer en JSON avec le serveur (arduino JSON ?).

  //todo : serveur HTTP qui écoute les requêtes entrantes pour les modifications de couleurs des LEDs. écouter le POST par exemple, et dès que le serveur IoT envoie un POST ici, mettre à jour l'état des LEDs
  // appli mobile envoie un POST au serveur javascript -> le serveur js transet le POST ici pour mettre à jour les données. Si OK -> réponse avec 200. Le serveur actualise ses données si réponse OK. Sinon erreur.
  
  
  //todo : quand tout est à ON et que l'utilisateur badge : les lumières s'éteignent, actualisation de l'état. Envoi des infos sur le serveur.
}
