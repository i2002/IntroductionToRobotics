#include "InputManager.h"
#include <new>
#include "context.h"


void InputManager::setupInput(InputActionType type) {
  actions = InputAction(type);
  actions.setupInput();
  actions.preview(state);
}

void InputManager::processMovement(JoystickPosition pos) {
  bool stateChanged = false;
  switch ((InputType) inputType) {
    case InputType::SELECT_INPUT:
      stateChanged = processSelectInputMovement(pos);
      break;

    case InputType::RANGE_INPUT:
      stateChanged = processRangeInputMovement(pos);
      break;

    case InputType::TEXT_INPUT:
      stateChanged = processTextInputMovement(pos);
      break;
  }

  if (stateChanged) {
    actions.preview(state);
  }
}

void InputManager::processActionBtn() {
  bool inputReturn = actions.inputReturn(state);
  actions.action(state);

  if (inputReturn) {
    appStateManager.stateTransition();
  }
}

void InputManager::setupSelectInput(const char* title, byte optionsSize, byte initialSelection) {
  new (&state.selectInput) SelectInput{title, optionsSize, initialSelection};
  inputType = (byte) InputType::SELECT_INPUT;
}

void InputManager::setupRangeInput(const char* title, byte initialValue) {
  new (&state.rangeInput) RangeInput{title, initialValue};
  inputType = (byte) InputType::RANGE_INPUT;
}

void InputManager::setupTextInput(const char* title, byte maxLen, const char* initialValue) {
  new (&state.textInput) TextInput{title, maxLen, initialValue};
  inputType = (byte) InputType::TEXT_INPUT;
}

bool InputManager::processSelectInputMovement(JoystickPosition pos) {
  if (pos == JoystickPosition::DOWN) {
    return state.selectInput.nextOption();
  } else if (pos == JoystickPosition::UP) {
    return state.selectInput.prevOption();
  }

  return false;
}

bool InputManager::processRangeInputMovement(JoystickPosition pos) {
  if (pos == JoystickPosition::LEFT) {
    return state.rangeInput.stepsDecrement();
  } else if (pos == JoystickPosition::RIGHT) {
    return state.rangeInput.stepsIncrement();
  }

  return false;
}

bool InputManager::processTextInputMovement(JoystickPosition pos) {
  if (pos == JoystickPosition::LEFT) {
    state.textInput.moveCursorLeft();
  } else if (pos == JoystickPosition::RIGHT) {
    state.textInput.moveCursorRight();
  } else if (pos == JoystickPosition::UP) {
    state.textInput.nextChar();
  } else if (pos == JoystickPosition::DOWN) {
    state.textInput.prevChar();
  }

  return true;
}
