#include "InputHandlers.h"
#include "context.h"
#include "resources/matrixImages.h"


void lcdBrightnessPreview(byte step) {
  statusDisp.setBrightness(step);
}

void lcdBrightnessAction(byte step) {
  statusDisp.setBrightness(step, true);
  menuManager.resumeMenu();
}

void matrixBrightnessPreview(byte step) {
  gameDisp.setBrightness(step);
}

void matrixBrightnessAction(byte step) {
  gameDisp.setBrightness(step, true);
  menuManager.resumeMenu();
}

void soundSettingPreview(byte val) {
  statusDisp.printMenuOption(val == 0 ? "Off" : "On");
}

void soundSettingAction(byte val) {
  // soundSettingStore.updateValue((bool)val); // FIXME: sound manager
  menuManager.resumeMenu();
}

void leaderboardPreview(byte val) {
  if (val == leaderboardSize) {
    gameDisp.displayImage(backImage);
    statusDisp.printMenuOption("Back");
  } else {
    gameDisp.displayImage(leaderboardImage);
    const HighscoreInfo& highscore = leaderboardManager.getHighscore(val);

    if (highscore.points == maxHighscorePoints) {
      statusDisp.printLeaderboard(val + 1, 0, "-----");
    } else {
      statusDisp.printLeaderboard(val + 1, highscore.points, highscore.name);
    }
  }
}

void leaderboardAction(byte val) {
  if (val == leaderboardSize) {
    menuManager.resumeMenu();
  }
}

void highscoreNameAction(const char* input) {
  leaderboardManager.setName(input);
  changeState(AppState::SAVE_HIGHSCORE);
}

void setupInput(InputType type) {
  switch (type) {
    case InputType::LCD_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("LCD Brightness", lcdBrightnessPreview, lcdBrightnessAction, statusDisp.getBrightness());

    case InputType::MATRIX_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("Matrix Brightness", matrixBrightnessPreview, matrixBrightnessAction, gameDisp.getBrightness());

    case InputType::SOUND_SETTING:
      return inputManager.setupSelectInput("Sounds", soundSettingPreview, soundSettingAction, 2, 0); // FIXME: sound manager

    case InputType::LEADERBOARD_VIEW:
      return inputManager.setupSelectInput("Leaderboard", leaderboardPreview, leaderboardAction, leaderboardSize + 1, 0);

    case InputType::HIGHSCORE_NAME:
      return inputManager.setupTextInput("Leaderboard name", nullptr, highscoreNameAction, leaderboardNameSize);
  }
}
