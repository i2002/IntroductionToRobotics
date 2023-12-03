#include "InputHandlers.h"
#include "context.h"
#include "resources/matrixImages.h"

const byte brightnessStep = 255 / RangeInput::maxSteps;

char labelBuf[17];

void lcdBrightnessPreview(byte step) {
  analogWrite(pinA, step * brightnessStep);
}

void lcdBrightnessAction(byte step) {
  lcdBrightnessStore.updateValue(step * brightnessStep);
  menuManager.resumeMenu();
}

void matrixBrightnessPreview(byte step) {
  Serial.println(step);
  gameDisp.updateIntensity(step);
}

void matrixBrightnessAction(byte step) {
  matrixBrightnessStore.updateValue(step);
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
    case InputType::LCD_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("LCD Brightness", lcdBrightnessPreview, lcdBrightnessAction, lcdBrightnessStore.readValue() / brightnessStep);

    case InputType::MATRIX_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("Matrix Brightness", matrixBrightnessPreview, matrixBrightnessAction, matrixBrightnessStore.readValue());

    case InputType::SOUND_SETTING:
      return inputManager.setupSelectInput("Sounds", soundSettingPreview, soundSettingAction, 2, soundSettingStore.readValue());

    case InputType::LEADERBOARD_VIEW:
      return inputManager.setupSelectInput("Leaderboard", leaderboardPreview, leaderboardAction, leaderboardSize + 1, 0);
  }
}
