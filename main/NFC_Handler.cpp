#include "NFC_Handler.h"

NFC_Handler::NFC_Handler(){}

String NFC_Handler::vReadNfc(NfcAdapter& nfc)
{ 
  if (nfc.tagPresent())
  {
      NfcTag tag = nfc.read();
      return tag.getUidString();
  }
  return "";
}

void NFC_Handler::handleBadging(String tag, HttpServer* httpServer, ChainableLED& leds)
{
    if(tag != "")
    {
      if (httpServer->isBadgeAuthorized(tag))
      {
        if(!httpServer->getObjectState()->getNfcState()->isBadgeActivated()) // We're in the case the NFC is "unbadged". User will now badge in order to enable the PIR.
        {
          Serial.println("Badge authorized. Welcome.");
          httpServer->getObjectState()->getNfcState()->vSetBadgeActivated(true);
          httpServer->getObjectState()->getPirState()->vSetActivated(true);
          httpServer->vPutStateToNodeExpressServer(); // "nfc_state" has its property "is_activated" true from now. It is sent to the server.
        }
        else if(httpServer->getObjectState()->getNfcState()->isBadgeActivated()) // We're in the case the NFC is "badged". PIR is enabled and LEDs may be enabled too.
        {
          Serial.println("Badge authorized. Good bye !");
          httpServer->getObjectState()->reset(); // reset state of all booleans values (nfc, pir & leds)
          leds.setColorRGB(0, 0, 0, 0); // turns off the led. But doesnt update the colors of the objectData reference. Here, the aim is to shut down the LEDs without changing its color state.
          httpServer->vPutStateToNodeExpressServer();
        }
        delay(NFC_READ_DELAY_AFTER_BADGING_ATTEMPT); // avoids "double badging" with a longer delay.
      }
      else
      {
        Serial.println("Badge unauthorized.");
      }
    }
    delay(NFC_READ_DELAY);
}
