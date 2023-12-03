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

void soundSettingPreview(byte val) {
  statusDisp.printMenuOption(val == 0 ? "Off" : "On");
}

void soundSettingAction(byte val) {
  soundSettingStore.updateValue((bool)val);
  menuManager.resumeMenu();
}

void leaderboardPreview(byte val) {
  if (val == leaderboardSize) {
    gameDisp.displayImage(backImage);
    statusDisp.printMenuOption("Back");
  } else {
    gameDisp.displayImage(leaderboardImage);
    statusDisp.printLeaderboard(val + 1, leaderboardStore.readValue(val));
  }
}

void leaderboardAction(byte val) {
  if (val == leaderboardSize) {
    menuManager.resumeMenu();
  }
}

void setupInput(InputType type) {
  switch (type) {
    case InputType::BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("Brightness", brightnessPreview, brightnessAction, brightnessStore.readValue() / brightnessStep);

    case InputType::SOUND_SETTING:
      return inputManager.setupSelectInput("Sounds", soundSettingPreview, soundSettingAction, 2, soundSettingStore.readValue());

    case InputType::LEADERBOARD_VIEW:
      return inputManager.setupSelectInput("Leaderboard", leaderboardPreview, leaderboardAction, leaderboardSize + 1, 0);
  }
}
