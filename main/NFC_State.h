#ifndef NFC_STATE_H
#define NFC_STATE_H

#include "Arduino.h"

class NFC_State
{
  private:
    boolean _isActivated;
     
  public:
    NFC_State();
    void vSwitchState();
    void vSetState(boolean activated);
    boolean getState();
};

#endif
