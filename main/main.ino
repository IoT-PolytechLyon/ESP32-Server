
#include <NfcAdapter.h>

#include <PN532/PN532_HSU/PN532_HSU.h>
PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc(pn532hsu);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("NFC Reader");
    nfc.begin();
}

void loop() {
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        tag.print();
    }
    delay(500);
}
