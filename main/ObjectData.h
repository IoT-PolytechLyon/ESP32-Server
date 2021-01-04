#ifndef OBJECTDATA_H
#define OBJECTDATA_H

#include <ArduinoJson.h>
#include "PIR_State.h"
#include "NFC_State.h"
#include "LED_State.h"

class ObjectData
{
  private:
    const char*   _id;
    NFC_State*    _nfcState;
    PIR_State*    _pirState;
    LED_State*    _ledState;

    void vInitObjects();
    
  public:
    ObjectData(String fromJson);
    ObjectData();
    String toJson();
    NFC_State* getNfcState();
    PIR_State* getPirState();
    LED_State* getLedState();
};

#endif
