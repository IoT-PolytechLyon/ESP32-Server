#include "NFC_State.h"

NFC_State::NFC_State()
{
  this->_isActivated = false;
}

void NFC_State::vSetBadgeActivated(boolean activated)
{
  this->_isActivated = activated;
}

boolean NFC_State::isBadgeActivated()
{
  return this->_isActivated;
}
