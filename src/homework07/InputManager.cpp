#include "InputManager.h"
#include <new>

InputManager::~InputManager() {
  destroyInputObject();
}

void InputManager::setupRangeInput(const char* title, InputCallback preview, InputCallback action, byte initialValue) {
  destroyInputObject();
  currentInput = new (inputsBuf) RangeInput{title, preview, action, initialValue};
}

void InputManager::setupSelectInput(const char* title, InputCallback preview, InputCallback action, byte optionsSize, byte initialSelection) {
  destroyInputObject();
  currentInput = new (inputsBuf) SelectInput{title, preview, action, optionsSize, initialSelection};
}

void InputManager::setupTextInput(const char* title, TextInputCallback preview, TextInputCallback action, byte maxLen, const char* initialValue) {
  destroyInputObject();
  currentInput = new (inputsBuf) TextInput{title, preview, action, maxLen, initialValue};
}

void InputManager::processMovement(JoystickPosition pos) {
  if (currentInput) {
    currentInput->processMovement(pos);
  }
}

void InputManager::processActionBtn() {
  if (currentInput) {
    currentInput->processActionBtn();
  }
}

void InputManager::destroyInputObject() {
  if (currentInput) {
    currentInput->~Input();
    currentInput = nullptr;
  }
}
