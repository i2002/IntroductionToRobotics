#include "RangeInput.h"
#include "context.h"

RangeInput::RangeInput(const char *title, InputCallback _preview, InputCallback _action, byte initialValue, InputCloseCallback _inputClose) {
  statusDisp.printTitle(title);
  statusDisp.printRange(initialValue);

  preview = _preview;
  action = _action;
  value = initialValue < maxSteps ? initialValue : 0;
  close = _inputClose;
}

void RangeInput::processMovement(JoystickPosition pos) {
  if (pos == JoystickPosition::LEFT && value > 0) {
    value--;
  } else if (pos == JoystickPosition::RIGHT && value < maxSteps) {
    value++;
  }

  statusDisp.printRange(value);
  
  if (preview) {
    preview(value);
  }
}

bool RangeInput::processActionBtn() {
  bool actionClose = true;
  if (close) {
    actionClose = close(value);
  }

  if (action) {
    action(value);
  }

  return actionClose;
}
