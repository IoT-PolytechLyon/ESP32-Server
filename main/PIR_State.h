#ifndef PIR_STATE_H
#define PIR_STATE_H

#include "Arduino.h"

class PIR_State
{
  private:
    boolean _detectedSomething;
     
  public:
    PIR_State();
    void vSwitchDetectedSomething();
    void vSetState(boolean detectedSomething);
    boolean hasDetectedSomething();
};

#endif
