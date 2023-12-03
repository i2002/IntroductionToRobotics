#include "RangeInput.h"

RangeInput::RangeInput(const char *title, InputCallback _preview, InputCallback _action, byte initialValue) {
  statusDisp.printTitle(title);
  statusDisp.printRange(initialValue);

  preview = _preview;
  action = _action;
  value = initialValue;
}

void RangeInput::processMovement(JoystickPosition pos) {
  if (pos == JoystickPosition::LEFT && value > 0) {
    value--;
  } else if (pos == JoystickPosition::RIGHT && value <= maxSteps) {
    value++;
  }

  statusDisp.printRange(value);
  
  if (preview) {
    preview(value);
  }
}

void RangeInput::processActionBtn() {
  if (action) {
    action(value);
  }
}
