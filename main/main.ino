/* NB : 
 * - WIFI credentials are located in the HttpServer.h file. Same goes for the NodeExpress server IP.
 * - NFC Reader must be plugged into UART port
 * - ...
 * - ...
 */

/* libs for main.ino */
#include "Arduino.h"
#include "NfcReader.h"
#include "HttpServer.h"
#include "ObjectData.h"

/* consts */
#define NFC_READ_DELAY          1000   // ms
#define NFC_READ_DELAY_OK       3000   // ms
#define SERVER_TIMEOUT_TIME     3000   // ms

/* global vars */
WiFiServer server(8080); // Server on port 8080. See https://randomnerdtutorials.com/esp32-web-server-arduino-ide/ for more details.
HttpServer* httpServer = new HttpServer(SERVER_TIMEOUT_TIME);
boolean nfcPassed = false;

/********************************** NFC Task *******************************************/
// Reads the NFC each NFC_READ_DELAY ms. If a NFC badge is recognized, it enables the PIR and waits for NFC_READ_DELAY_OK ms. 
// It then saves the current state on nodeExpress server side.

// If a NFC badge is recognized once again, it will shut down the LEDs & PIR and will save the new state on nodeExpress server side.
void vTaskReadNfc(void* pvParameters)
{
  NfcReader* reader = new NfcReader();
  PN532_HSU pn532hsu(Serial1);
  NfcAdapter nfc(pn532hsu);
  nfc.begin();
  for(;;)
  {
    String nfcTag = reader->vReadNfc(nfc);
    if(nfcTag != "")
    {
      Serial.println(nfcTag);
      if(!nfcPassed)
      {
        //todo : PERFORM GET to node express server.
        //checkNfc with database. If NFC ok -> nfcPassed = true.
        // POST to nodeExpress the new state.
        nfcPassed = true;
        delay(NFC_READ_DELAY_OK); // avoids "double badging"
      }
      else if(nfcPassed)
      {
        
      }
    }
    delay(NFC_READ_DELAY);
  }
}
/********************************** PIR Task *******************************************/
// The PIR task, enabled once a correct nfc badge was read by the program.
// when the PIR detects a movement, 
void vTaskPirHandler(void* pvParameters)
{
  for(;;)
  {
    if(nfcPassed)
    {
      
    }
  }
}
/********************************** Server Task ***************************************/
// Task that listens on port 8080 and that proceeds clients http requests.
void vTaskListenToIncomingConnections(void* pvParameters)
{
  server.begin(); // starts the server
  for(;;)
  {
    httpServer->vListenToIncomingConnections(server);
  }
}

/********************************** Main *********************************************/
void setup() 
{
  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("--IoT--\n");
  
  httpServer->vConnectToWiFi();
  httpServer->vInitializeObjectData();
  
  
  pinMode(LED_BUILTIN, OUTPUT);
 
  // JOB 1 : Starts NFC Reading.
  xTaskCreate(vTaskReadNfc, "vTaskReadNfc", 2000, NULL, 5, NULL);
  // JOB 2 : Starts the Http Server.
  //xTaskCreate(vTaskListenToIncomingConnections, "vTaskListenToIncomingConnections", 8000, NULL, 7, NULL);
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
