#ifndef STATUS_DISPLAY_H
#define STATUS_DISPLAY_H

#include <Arduino.h>
#include "LedControl.h"
#include "LiquidCrystal.h"
#include "config.h"

const byte UP_DOWN_ARROW = 0;
const byte UP_ARROW = 1;
const byte DOWN_ARROW = 2;
const byte LEFT_ARROW = 3;
const byte RIGHT_ARROW = 4;

class StatusDisplay {
  byte matrixBrightness = 2;
  LiquidCrystal lcd;

public:
  StatusDisplay() : lcd{rs, en, d4, d5, d6, d7} {}

  void setup();
  void setupMenu(const char *name);
  void printMenuOption(const char *name, bool canPrev, bool canNext);
  void printMessage(const char *message) {
    resetDisplay();
    lcd.print(message);
  }

  void setupGameInfo(int level);
  void updatePoints(int points);
  void printRange(int steps) {
    lcd.setCursor(0, 1);
    lcd.write(LEFT_ARROW);
    for (int i = 0; i < steps; i++) {
      lcd.write(0b11111111);
    }
    for (int i = steps; i < 14; i++) {
      lcd.write(' ');
    }

    lcd.write(RIGHT_ARROW);
  }

private:
  void resetDisplay();
  void printMenuArrow(bool canPrev, bool canNext);
};

#endif // STATUS_DISPLAY_H
