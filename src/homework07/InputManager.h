#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "Joystick.h"
#include "RangeInput.h"
#include "SelectInput.h"
#include "TextInput.h"
#include "InputAction.h"

enum class InputType {
  SELECT_INPUT,
  RANGE_INPUT,
  TEXT_INPUT
};

union InputState {
  SelectInput selectInput;
  RangeInput rangeInput;
  TextInput textInput;

  InputState() {
    memset(this, 0, sizeof(*this));
  }
};

class InputManager {
  byte inputType;
  InputState state;
  InputAction actions;

public:
  void setupInput(InputActionType type);

  void processMovement(JoystickPosition pos);

  void processActionBtn();

private:
  friend class InputAction;
  friend class MenuManager;

  void setupRangeInput(const char* title, byte initialValue = 0);

  void setupSelectInput(const char* title, byte optionsSize, byte initialSelection = 0);

  void setupTextInput(const char* title, byte maxLen, const char* initialValue = nullptr);

  bool processSelectInputMovement(JoystickPosition pos);

  bool processRangeInputMovement(JoystickPosition pos);

  bool processTextInputMovement(JoystickPosition pos);
};

#endif // INPUT_MANAGER_H
