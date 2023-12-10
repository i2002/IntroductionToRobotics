#ifndef SELECT_INPUT_H
#define SELECT_INPUT_H
#include "Input.h"

typedef bool (*InputCloseCallback)(byte);

struct SelectInput : Input {
  byte optionsSize;
  byte currentOption;
  InputCallback preview;
  InputCallback action;
  InputCloseCallback close;

public:
  SelectInput(const char *title, InputCallback _preview, InputCallback _action, byte _optionsSize, byte initialSelection, InputCloseCallback _close);
  ~SelectInput() = default;

  void processMovement(JoystickPosition pos);

  bool processActionBtn();

private:
  void printCurrentOption();

  bool canNext();

  bool canPrev();
};

#endif // SELECT_INPUT_H
