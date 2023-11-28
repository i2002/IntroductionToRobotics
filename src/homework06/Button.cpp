#include "Button.h"

void Button::setup() {
  digitalWrite(buttonPin, INPUT_PULLUP);
}

bool Button::buttonPressed () {
  int buttonValue = digitalRead(buttonPin);

  // reset debounce timer if value changed too quick
  if (buttonValue != lastButtonRead) {
    lastButtonDebounce = millis();
  }
  lastButtonRead = buttonValue;

  // value stable in debounce interval
  if (millis() - lastButtonDebounce > buttonDebounceInterval) {
    if (buttonValue != lastButtonState) {
      lastButtonState = buttonValue;
      if (buttonValue == LOW) {
        return true;
      }
    }
  }

  return false;
}
