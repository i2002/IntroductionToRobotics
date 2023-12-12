#ifndef DISPLAY_SCREENS_H
#define DISPLAY_SCREENS_H
#include "../StatusDisplay.h"

const Screen welcomeScreen = {
  "Bomber game (v1)",
};

const Screen lostGameScreen = {
  "You lost :(",
  "Try again      >"
};

const Screen wonGameScreen = {
  "You won :)",
  "Points:        >"
};

const Screen aboutScreen = {
  "(c) 2023",
  "Tudor Butufei"
};

#endif // DISPLAY_SCREENS_H
