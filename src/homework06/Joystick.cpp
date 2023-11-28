#include "Joystick.h"

void Joystick::setup() {
  pinMode(joystickPinX, INPUT);
  pinMode(joystickPinY, INPUT);
}

bool Joystick::processMovement() {
  // read joystick status
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);
  bool stateChanged = false;

  // joystick returned to neutral
  bool returnedFromUp = currentJoystickState == JoystickPosition::UP && checkReturn(joystickY, isUpperThreshold(JoystickPosition::UP));
  bool returnedFromDown = currentJoystickState == JoystickPosition::DOWN && checkReturn(joystickY, isUpperThreshold(JoystickPosition::DOWN));
  bool returnedFromLeft = currentJoystickState == JoystickPosition::LEFT && checkReturn(joystickX, isUpperThreshold(JoystickPosition::LEFT));
  bool returnedFromRight = currentJoystickState == JoystickPosition::RIGHT && checkReturn(joystickX, isUpperThreshold(JoystickPosition::RIGHT));
  bool joystickReturned = returnedFromUp || returnedFromDown || returnedFromRight || returnedFromLeft;

  if (joystickReturned) {
    currentJoystickState = JoystickPosition::NEUTRAL;
    stateChanged = true;
  }

  // movement only from neutral position
  if (currentJoystickState == JoystickPosition::NEUTRAL || joystickReturned) {
    if (checkThreshold(joystickY, isUpperThreshold(JoystickPosition::UP))) {
      currentJoystickState = JoystickPosition::UP;
      stateChanged = true;
    } else if (checkThreshold(joystickY, isUpperThreshold(JoystickPosition::DOWN))) {
      currentJoystickState = JoystickPosition::DOWN;
      stateChanged = true;
    } else if (checkThreshold(joystickX, isUpperThreshold(JoystickPosition::LEFT))) {
      currentJoystickState = JoystickPosition::LEFT;
      stateChanged = true;
    } else if (checkThreshold(joystickX, isUpperThreshold(JoystickPosition::RIGHT))) {
      currentJoystickState = JoystickPosition::RIGHT;
      stateChanged = true;
    }
  }

  return stateChanged;
}

JoystickPosition Joystick::getState() {
  return currentJoystickState;
}

bool Joystick::checkThreshold(int value, bool upper) {
  return upper ? (value > joystickUpperThreshold) : (value < joystickLowerThreshold);
}

bool Joystick::checkReturn(int value, bool upper) {
  return upper ? (value < joystickUpperThreshold - joystickReturnThreshlold) : (value > joystickLowerThreshold + joystickReturnThreshlold);
}

bool Joystick::isUpperThreshold(JoystickPosition pos) {
  switch(pos) {
    case JoystickPosition::UP:
      return invertY;
    case JoystickPosition::DOWN:
      return !invertY;
    case JoystickPosition::LEFT:
      return invertX;
    case JoystickPosition::RIGHT:
      return !invertX;
  }

  return false;
}
