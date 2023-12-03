#include "SelectInput.h"

SelectInput::SelectInput(const char *title, InputCallback _preview, InputCallback _action, SelectLabelCallback _labelCallback, byte _optionsSize, byte initialSelection) {
  preview = _preview;
  action = _action;
  labelCallback = _labelCallback;
  optionsSize = _optionsSize;
  currentOption = initialSelection;

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

void SelectInput::processActionBtn() {
  if (action) {
    action(currentOption);
  }
}

void SelectInput::printCurrentOption() {
  if (labelCallback) {
    statusDisp.printMenuOption(labelCallback(currentOption), canPrev(), canNext());
  }

  if (preview) {
    preview(currentOption);
  }
}

bool SelectInput::canNext() {
  return currentOption < optionsSize - 1;
}

bool SelectInput::canPrev() {
  return currentOption > 0;
}
