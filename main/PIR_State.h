#ifndef PIR_STATE_H
#define PIR_STATE_H

#include "Arduino.h"

class PIR_State
{
  private:
    boolean _detectedSomething;
    boolean _isActivated;
     
  public:
    PIR_State();
    void vSetDetectedSomething(boolean detectedSomething);
    boolean hasDetectedSomething();
    void vSetActivated(boolean activated);
    boolean isActivated();
};

#endif
