#include "RangeInput.h"
#include "context.h"

RangeInput::RangeInput(const char *title, byte initialValue) {
  statusDisp.printTitle(title);
  statusDisp.printRange(initialValue);

  value = initialValue < maxSteps ? initialValue : 0;
}

bool RangeInput::stepsIncrement() {
  if (value >= maxSteps) {
    return false;
  }

  value++;
  statusDisp.printRange(value);
  return true;
}

bool RangeInput::stepsDecrement() {
  if (value <= 0) {
    return false;
  }

  value--;
  statusDisp.printRange(value);
  return true;
}

byte RangeInput::getValue() const {
  return value;
}
