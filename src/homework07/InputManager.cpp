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
