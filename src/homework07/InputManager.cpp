#include "InputManager.h"
#include <new>
#include "context.h"

InputManager::~InputManager() {
  destroyInputObject();
}

void InputManager::setupRangeInput(const char* title, InputCallback preview, InputCallback action, byte initialValue, InputCloseCallback close) {
  destroyInputObject();
  currentInput = new (inputsBuf) RangeInput{title, preview, action, initialValue, close};
}

void InputManager::setupSelectInput(const char* title, InputCallback preview, InputCallback action, byte optionsSize, byte initialSelection, InputCloseCallback close) {
  destroyInputObject();
  currentInput = new (inputsBuf) SelectInput{title, preview, action, optionsSize, initialSelection, close};
}

void InputManager::setupTextInput(const char* title, TextInputCallback preview, TextInputCallback action, byte maxLen, const char* initialValue, TextInputCloseCallback close) {
  destroyInputObject();
  currentInput = new (inputsBuf) TextInput{title, preview, action, maxLen, initialValue, close};
}

void InputManager::processMovement(JoystickPosition pos) {
  if (currentInput) {
    currentInput->processMovement(pos);
  }
}

void InputManager::processActionBtn() {
  if (currentInput) {
    if (currentInput->processActionBtn()) {
      appStateManager.stateTransition();
    }
  }
}

void InputManager::destroyInputObject() {
  if (currentInput) {
    currentInput->~Input();
    currentInput = nullptr;
  }
}
