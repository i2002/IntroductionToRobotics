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

typedef const char* (Screen)[2];

class StatusDisplay {
  LiquidCrystal lcd;

public:
  StatusDisplay() : lcd{rs, en, d4, d5, d6, d7} {}

  void setup();
  void printTitle(const char *name);
  void printMenuOption(const char *name);
  void printRange(int step);
  void printScreen(const Screen screen);
  void printLeaderboard(byte place, byte score);
  void printInputChar(byte cursor, char inputChar);
  void printMenuArrow(bool canPrev = true, bool canNext = true);

  void setupGameInfo(int level);
  void updatePoints(int points);

private:
  void resetDisplay();
  void printBlank(byte from = 0, byte to = 16, byte line = 1);
};

#endif // STATUS_DISPLAY_H
