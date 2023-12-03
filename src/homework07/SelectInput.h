#ifndef SELECT_INPUT_H
#define SELECT_INPUT_H
#include "Input.h"
#include "StatusDisplay.h"

extern StatusDisplay statusDisp;

typedef const char* (*SelectLabelCallback)(byte);

struct SelectInput : Input {
  byte optionsSize;
  byte currentOption;
  SelectLabelCallback labelCallback;
  InputCallback preview;
  InputCallback action;

public:
  SelectInput(const char *title, InputCallback _preview, InputCallback _action, SelectLabelCallback _labelCallback, byte _optionsSize, byte initialSelection);
  ~SelectInput() = default;

  void processMovement(JoystickPosition pos);

  void processActionBtn();

private:
  void printCurrentOption();

  bool canNext();

  bool canPrev();
};

#endif // SELECT_INPUT_H
