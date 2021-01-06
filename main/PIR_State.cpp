#include "PIR_State.h"

PIR_State::PIR_State()
{
  this->_detectedSomething = false;
  this->_isActivated = false;
}

void PIR_State::vSetDetectedSomething(boolean detectedSomething)
{
  this->_detectedSomething = detectedSomething;
}

boolean PIR_State::hasDetectedSomething()
{
  return this->_detectedSomething;
}

void PIR_State::vSetActivated(boolean activated)
{
  this->_isActivated = activated;
}

boolean PIR_State::isActivated()
{
  return this->_isActivated;
}
