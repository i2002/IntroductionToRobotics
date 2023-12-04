#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Input.h"
#include "RangeInput.h"
#include "SelectInput.h"
#include "TextInput.h"

class InputManager {
  Input *currentInput = nullptr;
  char inputsBuf[max(max(sizeof(RangeInput), sizeof(SelectInput)), sizeof(TextInput))];

public:
  ~InputManager();

  void setupRangeInput(const char* title, InputCallback preview, InputCallback action, byte initialValue = 0);

  void setupSelectInput(const char* title, InputCallback preview, InputCallback action, byte optionsSize, byte initialSelection = 0);

  void setupTextInput(const char* title, TextInputCallback preview, TextInputCallback action, char* inputBuf, byte maxLen);

  void processMovement(JoystickPosition pos);

  void processActionBtn();

  void destroyInputObject();
};

#endif // INPUT_MANAGER_H
