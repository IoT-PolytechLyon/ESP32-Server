/* NB : 
 * - WIFI credentials are located in the HttpServer.h file. Same goes for the NodeExpress server IP.
 * - NFC Reader must be plugged into UART pin
 * - Motion sensor must be plugged into D4-D5
 * - LED must be plugged into D2-D3
 * - ...
 */

/* libs for main.ino */
#include <ChainableLED.h> // TODO : refactor into a led manager class ?
#include "Arduino.h"
#include "NfcReader.h"
#include "HttpServer.h"
#include "ObjectData.h"

/* consts */ // TODO : put them into a header file.
#define NFC_READ_DELAY                             1000   // ms
#define NFC_READ_DELAY_AFTER_BADGING_ATTEMPT       3000   // ms
#define SERVER_TIMEOUT_TIME                        3000   // ms
#define PIR_TIMEOUT_TIME                           5000   // ms
#define PIR_DETECTION_DELAY                        50     // ms
#define LED_UPDATE_DELAY                           50     // ms

#define MOTION_SENSOR_D4                           15     // pin number
#define LED_D2                                     14     // pin number
#define LED_D3                                     32     // pin number


/* global vars */
WiFiServer server(8080); // Server on port 8080. See https://randomnerdtutorials.com/esp32-web-server-arduino-ide/ for more details.
HttpServer* httpServer = new HttpServer(SERVER_TIMEOUT_TIME);
ChainableLED leds(LED_D2, LED_D3, 1);


// void triggered when movement is detected (interruption)
void IRAM_ATTR movementDetection()
{  
  // when badge is activated and lights are off, we can begin detection. If something is detected, the task pirState will proceed and enable the leds.
  if(httpServer->getObjectState()->getNfcState()->isBadgeActivated() && !httpServer->getObjectState()->getLedState()->isOn())
  {
    Serial.println("Motion detected !");
    httpServer->getObjectState()->getPirState()->vSetDetectedSomething(true);
  }
}
/********************************** NFC Task *******************************************/
// Reads the NFC each NFC_READ_DELAY ms. If a NFC badge is recognized, it enables the PIR and waits for NFC_READ_DELAY_OK ms. 
// It then saves the current state on nodeExpress server side.
// If a NFC badge is recognized once again, it will shut down the LEDs & PIR and will save the new state on nodeExpress server side.
void vTaskNfcHandler(void* pvParameters)
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
      if(!httpServer->getObjectState()->getNfcState()->isBadgeActivated()) // We're in the case the NFC is "unbadged". User will now badge in order to enable the PIR.
      {
        if (httpServer->isBadgeAuthorized(nfcTag))
        {
          Serial.println("Badge authorized. Welcome.");
          httpServer->getObjectState()->getNfcState()->vSetBadgeActivated(true);
          httpServer->vPutStateToNodeExpressServer(); // "nfc_state" has its property "is_activated" true from now. It is sent to server.
          httpServer->getObjectState()->getPirState()->vSetActivated(true);
        }
        else
        {
          Serial.println("Badge unauthorized.");
        }
        delay(NFC_READ_DELAY_AFTER_BADGING_ATTEMPT); // avoids "double badging" with a longer delay.
      }
      else if(httpServer->getObjectState()->getNfcState()->isBadgeActivated()) // We're in the case the NFC is "badged". PIR is enabled and LEDs may be enabled too.
      {
        Serial.println("Badge authorized. Good bye !");
        httpServer->getObjectState()->reset(); // reset state of all booleans values (nfc, pir & leds)
        leds.setColorRGB(0, 0, 0, 0); // turn off the led
        httpServer->vPutStateToNodeExpressServer();
        delay(NFC_READ_DELAY_AFTER_BADGING_ATTEMPT);
      }
    }
    delay(NFC_READ_DELAY);
  }
}
/********************************** PIR Task *******************************************/
// The PIR task, enabled once a correct nfc badge was read by the program.
// when the PIR detects a movement, it turns on the LEDs.
void vTaskPirSensorHandler(void* pvParameters)
{
  for(;;)
  {
    // triggered when PIR was enabled by the vTaskReadNfc method & when motion is detected.
    if(httpServer->getObjectState()->getPirState()->isActivated() && httpServer->getObjectState()->getPirState()->hasDetectedSomething()) 
    {
      if(!httpServer->getObjectState()->getLedState()->isOn())
      {
        Serial.println("LEDs enabled !");
        httpServer->getObjectState()->getLedState()->vSetIsOn(true);
        httpServer->vPutStateToNodeExpressServer();
      }
    }
    delay(PIR_DETECTION_DELAY);
  }
}
/********************************** LEDs Task *******************************************/
void vTaskLedsHandler(void* pvParameters)
{
  for(;;)
  {
    if(httpServer->getObjectState()->getLedState()->isOn())
    {
      // sets led colors according to content of object data continuously.
      LED_State* state = httpServer->getObjectState()->getLedState();
      leds.setColorRGB(0, state->getRed(), state->getGreen(), state->getBlue());
    }
    delay(LED_UPDATE_DELAY);
  }
}
/********************************** Server Task *****************************************/
// Task that listens on port 8080 and that proceeds clients http requests.
void vTaskIncomingConnectionsHandler(void* pvParameters)
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
  leds.setColorRGB(0, 0, 0, 0); // turn off the leds at the initial state.
  Serial.begin(115200);
  while(!Serial);
  Serial.println("--IoT--\n");
  
  httpServer->vConnectToWiFi();
  httpServer->vInitializeObjectData();
  
  pinMode(LED_BUILTIN, OUTPUT);   // set ups the built-in led
  pinMode(MOTION_SENSOR_D4, INPUT_PULLUP); // sets up the motion sensor
  attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR_D4), movementDetection, RISING); // attaches the movementDetection method to the MOTION_SENSOR pin.
 
  // JOB 1 : Starts NFC Reading.
  xTaskCreate(vTaskNfcHandler, "vTaskNfcHandler", 3500, NULL, 5, NULL);
  // JOB 2 : Starts the PIR sensor task
  xTaskCreate(vTaskPirSensorHandler, "vTaskPirSensorHandler", 3500, NULL, 5, NULL);
  // JOB 3 : Starts the LEDs task
  xTaskCreate(vTaskLedsHandler, "vTaskLedsHandler", 1000, NULL, 1, NULL);
  // JOB 4 : Starts the Http Server so that incoming HTTP requests can be processed.
  xTaskCreate(vTaskIncomingConnectionsHandler, "vTaskIncomingConnectionsHandler", 5000, NULL, 7, NULL);
}

void loop() { }
