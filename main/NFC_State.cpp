#include "NFC_State.h"

NFC_State::NFC_State()
{
  this->_isActivated = false;
}

void NFC_State::vSwitchState()
{ 
  if(this->_isActivated)
  {
    this->_isActivated = false;
  }
  else
  {
    this->_isActivated = true;
  }
}

void NFC_State::vSetState(boolean activated)
{
  this->_isActivated = activated;
}

boolean NFC_State::getState()
{
  return this->_isActivated;
}
