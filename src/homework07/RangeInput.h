#ifndef RANGE_INPUT_H
#define RANGE_INPUT_H
#include "Input.h"

typedef bool (*InputCloseCallback)(byte);

class RangeInput : public Input {
  InputCallback preview = nullptr;
  InputCallback action = nullptr;
  InputCloseCallback close = nullptr;
  byte value = 0;

public:
  static const byte maxSteps = 14;
  
  RangeInput(const char *title, InputCallback _preview, InputCallback _action, byte initialValue, InputCloseCallback _inputClose);
  ~RangeInput() = default;

  void processMovement(JoystickPosition pos);

  bool processActionBtn();
};

#endif // RANGE_INPUT_H
