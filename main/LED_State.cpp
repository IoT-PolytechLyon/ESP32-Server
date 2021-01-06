#include "LED_State.h"

LED_State::LED_State()
{
  this->_isOn = false;
  this->_red = 0;
  this->_green = 0;
  this->_blue = 0;
}

void LED_State::vSetIsOn(boolean isOn)
{
  this->_isOn = isOn;
}

void LED_State::vSetRed(int red)
{
  this->_red = red;
}

void LED_State::vSetGreen(int green)
{
  this->_green = green;
}

void LED_State::vSetBlue(int blue)
{
  this->_blue = blue;
}

boolean LED_State::isOn()
{
  return this->_isOn;
}

int LED_State::getRed()
{
  return this->_red;
}

int LED_State::getGreen()
{
  return this->_green;
}

int LED_State::getBlue()
{
  return this->_blue;
}
