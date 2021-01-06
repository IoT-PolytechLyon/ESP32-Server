#ifndef LED_STATE_H
#define LED_STATE_H

#include "Arduino.h"

class LED_State
{
  private:
    boolean _isOn;
    int _red;
    int _green;
    int _blue;
     
  public:
    LED_State();
    void vSetIsOn(boolean isOn);
    void vSetRed(int red);
    void vSetGreen(int green);
    void vSetBlue(int blue);
    boolean isOn();
    int getRed();
    int getGreen();
    int getBlue();
};

#endif
