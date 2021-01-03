#include "NfcReader.h"

NfcReader::NfcReader(){}

String NfcReader::vReadNfc(NfcAdapter& nfc)
{ 
  if (nfc.tagPresent())
  {
      NfcTag tag = nfc.read();
      return tag.getUidString();
  }
  return "";
}
