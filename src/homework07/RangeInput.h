#ifndef RANGE_INPUT_H
#define RANGE_INPUT_H
#include "Input.h"

class RangeInput : public Input {
  InputCallback preview = nullptr;
  InputCallback action = nullptr;
  byte value = 0;

public:
  static const byte maxSteps = 14;
  
  RangeInput(const char *title, InputCallback _preview, InputCallback _action, byte initialValue);
  ~RangeInput() = default;

  void processMovement(JoystickPosition pos);

  void processActionBtn();
};

#endif // RANGE_INPUT_H
