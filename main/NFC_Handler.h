#ifndef NFCREADER_H
#define NFCREADER_H


#include <NfcAdapter.h>
#include <ChainableLED.h>

#include "Arduino.h"
#include "HttpServer.h"
#include "Consts.h"

class NFC_Handler
{
  public:
    NFC_Handler();
    String vReadNfc(NfcAdapter& nfc);
    void handleBadging(String tag, HttpServer* httpServer, ChainableLED& leds);
};

#endif
