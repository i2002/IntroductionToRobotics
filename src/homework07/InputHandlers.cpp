#include "InputHandlers.h"
#include "context.h"
#include "resources/matrixImages.h"

const byte brightnessStep = 255 / RangeInput::maxSteps;

char labelBuf[17];

void brightnessPreview(byte step) {
  analogWrite(pinA, step * brightnessStep);
}

void brightnessAction(byte step) {
  brightnessStore.updateValue(step * brightnessStep);
  menuManager.resumeMenu();
}

void soundSettingAction(byte val) {
  soundSettingStore.updateValue((bool)val);
  menuManager.resumeMenu();
}

const char* soundSettingLabel(byte val) {
  Serial.println(val);
  return val == 0 ? "Off" : "On";
}

void leaderboardAction(byte val) {
  if (val == leaderboardSize) {
    menuManager.resumeMenu();
  }
}

void leaderboardPreview(byte val) {
  if (val == leaderboardSize) {
    gameDisp.displayImage(backImage);
  } else {
    gameDisp.displayImage(leaderboardImage);
  }
}

const char *leaderboardLabel(byte val) {
  if (val == leaderboardSize) {
    return "Back";
  } else {
    byte highscore = leaderboardStore.readValue(val);
    labelBuf[0] = val + 1 + '0';
    labelBuf[1] = '.';
    itoa(highscore, labelBuf + 2, 10);
    return labelBuf;
  }
}

void setupInput(InputType type) {
  switch (type) {
    case InputType::BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("Brightness", brightnessPreview, brightnessAction, brightnessStore.readValue() / brightnessStep);

    case InputType::SOUND_SETTING:
      return inputManager.setupSelectInput("Sounds", nullptr, soundSettingAction, soundSettingLabel, 2, soundSettingStore.readValue());

    case InputType::LEADERBOARD_VIEW:
      return inputManager.setupSelectInput("Leaderboard", leaderboardPreview, leaderboardAction, leaderboardLabel, leaderboardSize + 1, 0);
  }
}
