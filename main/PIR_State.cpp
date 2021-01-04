#include "PIR_State.h"

PIR_State::PIR_State()
{
  this->_detectedSomething = false;
}

void PIR_State::vSwitchDetectedSomething()
{ 
  if(this->_detectedSomething)
  {
    this->_detectedSomething = false;
  }
  else
  {
    this->_detectedSomething = true;
  }
}

void PIR_State::vSetState(boolean detectedSomething)
{
  this->_detectedSomething = detectedSomething;
}

boolean PIR_State::hasDetectedSomething()
{
  return this->_detectedSomething;
}
