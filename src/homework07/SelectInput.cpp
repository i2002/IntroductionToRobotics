#include "SelectInput.h"
#include "context.h"

SelectInput::SelectInput(const char *title, InputCallback _preview, InputCallback _action, byte _optionsSize, byte initialSelection, InputCloseCallback _close) {
  preview = _preview;
  action = _action;
  optionsSize = _optionsSize;
  currentOption = initialSelection < optionsSize ? initialSelection : 0;
  close = _close;

  statusDisp.printTitle(title);
  printCurrentOption();
}

void SelectInput::processMovement(JoystickPosition pos) {
  switch (pos) {
    case JoystickPosition::LEFT:
    case JoystickPosition::RIGHT:
    case JoystickPosition::NEUTRAL:
      return;

    case JoystickPosition::UP:
      if (!canPrev()) {
        return;
      }

      currentOption--;
      break;

    case JoystickPosition::DOWN:
      if (!canNext()) {
        return;
      }

      currentOption++;
      break;
  }

  printCurrentOption();
}

bool SelectInput::processActionBtn() {
  bool actionClose = true;
  if (close) {
    actionClose = close(currentOption);
  }

  if (action) {
    action(currentOption);
  }

  return actionClose;
}

void SelectInput::printCurrentOption() {
  if (preview) {
    preview(currentOption);
  }
  statusDisp.printMenuArrow(canPrev(), canNext());
}

bool SelectInput::canNext() {
  return currentOption < optionsSize - 1;
}

bool SelectInput::canPrev() {
  return currentOption > 0;
}
