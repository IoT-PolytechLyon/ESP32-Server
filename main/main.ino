#include <ChainableLED.h>
#include "Consts.h"
#include "Arduino.h"
#include "NFC_Handler.h"
#include <PN532/PN532_HSU/PN532_HSU.h>
#include "HttpServer.h"
#include "ObjectData.h"

/* global vars */
WiFiServer server(8080); // Server on port 8080. See https://randomnerdtutorials.com/esp32-web-server-arduino-ide/ for more details.
HttpServer* httpServer = new HttpServer(SERVER_TIMEOUT_TIME);
ChainableLED leds(LED_D2, LED_D3, 1);


void IRAM_ATTR movementDetection() // void triggered when movement is detected (interruption)
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
  NFC_Handler* reader = new NFC_Handler();
  PN532_HSU pn532hsu(Serial1);
  NfcAdapter nfc(pn532hsu);
  nfc.begin();
  for(;;)
  {
    String nfcTag = reader->vReadNfc(nfc);
    reader->handleBadging(nfcTag, httpServer, leds);
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
