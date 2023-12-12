#ifndef RANGE_INPUT_H
#define RANGE_INPUT_H
#include <Arduino.h>

class RangeInput {
  byte value = 0;

public:
  static const byte maxSteps = 14;
  
  RangeInput(const char *title = nullptr, byte initialValue = 0);

  bool stepsIncrement();

  bool stepsDecrement();

  byte getValue() const;
};

#endif // RANGE_INPUT_H
