/* libs */
#include "Arduino.h"
#include <NfcAdapter.h>
#include <PN532/PN532_HSU/PN532_HSU.h>

/* consts */
#define NFC_READ_DELAY    500 //ms

/* global vars */
PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc(pn532hsu);

/********************************** NfcReader Class **********************************/
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

/********************************** Main *******************************************/

NfcReader reader = NfcReader(NFC_READ_DELAY);

void vTaskReadNfc(void *pvParameters)
{
  for(;;)
  {
    reader.vReadNfc();
  }
}

void setup() 
{
    Serial.begin(115200);
    while(!Serial);
    Serial.println("--IoT--");

    // JOB 1 : Starts NFC Reading.
    xTaskCreate(vTaskReadNfc, "vTaskReadNfc", 1000, NULL, 5, NULL);
}

void loop() 
{

  //todo : connexion wifi. Dans un JOB
  //todo : si un badge est lu -> vérifier s'il correspond à un badge en BD. -> si oui, activer le PIR. PIR dans un JOB.
  //todo : si PIR detecte un mouvement -> regarder si un état est sauvegardé en BD -> si oui, l'utiliser pour allumer les LEDs, sinon, PUT un état par défaut (blanc) et l'utiliser (et allumer les leds).

  //todo : communiquer en JSON avec le serveur (arduino JSON ?).

  //todo : serveur HTTP qui écoute les requêtes entrantes pour les modifications de couleurs des LEDs. écouter le POST par exemple, et dès que le serveur IoT envoie un POST ici, mettre à jour l'état des LEDs
  // appli mobile envoie un POST au serveur javascript -> le serveur js transet le POST ici pour mettre à jour les données. Si OK -> réponse avec 200. Le serveur actualise ses données si réponse OK. Sinon erreur.
  
  
  //todo : quand tout est à ON et que l'utilisateur badge : les lumières s'éteignent, actualisation de l'état. Envoi des infos sur le serveur.
}
