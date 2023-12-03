#ifndef INPUT_H
#define INPUT_H
#include "Joystick.h"

typedef void (*InputCallback)(byte);

class Input {
public:
  virtual void processMovement(JoystickPosition pos) = 0;
  virtual void processActionBtn() = 0;
  virtual ~Input() {}
};

#endif // INPUT_H
