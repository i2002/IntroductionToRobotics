#ifndef SELECT_INPUT_H
#define SELECT_INPUT_H
#include <Arduino.h>

struct SelectInput {
  byte optionsSize = 0;
  byte currentOption = 0;

public:
  SelectInput(const char *title = nullptr, byte _optionsSize = 0, byte initialSelection = 0);

  bool nextOption();

  bool prevOption();

  byte getCurrentOption() const;

private:
  void printCurrentOption();

  bool canNext();

  bool canPrev();
};

#endif // SELECT_INPUT_H
