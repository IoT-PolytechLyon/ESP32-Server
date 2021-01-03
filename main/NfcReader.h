#ifndef NFCREADER_H
#define NFCREADER_H

#include "Arduino.h"
#include <NfcAdapter.h>
#include <PN532/PN532_HSU/PN532_HSU.h>

class NfcReader
{
  public:
    NfcReader();
    String vReadNfc(NfcAdapter& nfc);
};

#endif
