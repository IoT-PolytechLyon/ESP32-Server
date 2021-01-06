#include "ObjectData.h"

ObjectData::ObjectData(String fromJson)
{
  this->vInitObjects();
  
  StaticJsonDocument<1300> doc;
  deserializeJson(doc, fromJson);

  // nfc and pir are at false by default when object is constructed. User did not badged yet.
  // same goes for led state (on/off). It will be switched on later on when user badged+PIR detected something.
  // we just retrieve rgb values 
  this->_ledState->vSetRed(doc[0]["state"]["led_state"]["red_value"]);
  this->_ledState->vSetGreen(doc[0]["state"]["led_state"]["green_value"]);
  this->_ledState->vSetBlue(doc[0]["state"]["led_state"]["blue_value"]);
  // and id
  this->_id = doc[0]["_id"];
}

ObjectData::ObjectData()
{
  this->_id = 0;
  this->vInitObjects();
}

void ObjectData::reset()
{
  this->_nfcState->vSetBadgeActivated(false);
  this->_pirState->vSetActivated(false);
  this->_pirState->vSetDetectedSomething(false);
  this->_ledState->vSetIsOn(false);
}

void ObjectData::vInitObjects()
{
  this->_nfcState = new NFC_State();
  this->_pirState = new PIR_State();
  this->_ledState = new LED_State();
}

NFC_State* ObjectData::getNfcState()
{
  return this->_nfcState;
}

PIR_State* ObjectData::getPirState()
{
  return this->_pirState;
}

LED_State* ObjectData::getLedState()
{
  return this->_ledState;
}


String ObjectData::toJson()
{
  StaticJsonDocument<1300> doc;
  doc["_id"] = this->_id;
  doc["state"]["pir_state"]["detected_something"] = this->_pirState->hasDetectedSomething();
  doc["state"]["nfc_state"]["is_activated"] = this->_nfcState->isBadgeActivated();
  doc["state"]["led_state"]["is_on"] = this->_ledState->isOn();
  doc["state"]["led_state"]["red_value"] = this->_ledState->getRed();
  doc["state"]["led_state"]["green_value"] = this->_ledState->getGreen();
  doc["state"]["led_state"]["blue_value"] = this->_ledState->getBlue();
  String result = "";
  serializeJson(doc, result);
  return result;
}

const char* ObjectData::getId()
{
  return this->_id;
}
