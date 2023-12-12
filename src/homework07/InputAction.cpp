#include "InputAction.h"
#include "context.h"
#include "resources/matrixImages.h"

InputAction::InputAction(InputActionType _type): type{(byte)_type} {}

void InputAction::setupInput() {
  switch ((InputActionType) type) {
    case InputActionType::MENU_INPUT:
      return menuManager.menuInputSetup();

    case InputActionType::LCD_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("LCD Brightness", statusDisp.getBrightness());

    case InputActionType::MATRIX_BRIGHTNESS_SETTING:
      return inputManager.setupRangeInput("Matrix Brightness", gameDisp.getBrightness());

    case InputActionType::SOUND_SETTING:
      return inputManager.setupSelectInput("Sounds", 2, (byte) soundManager.getEnabled());

    case InputActionType::LEADERBOARD_VIEW:
      return inputManager.setupSelectInput("Leaderboard", leaderboardSize + 1, 0);

    case InputActionType::HIGHSCORE_NAME:
      return inputManager.setupTextInput("Leaderboard name", leaderboardNameSize);
  }
}

void InputAction::preview(const InputState &state) {
  switch ((InputActionType) type) {
    case InputActionType::MENU_INPUT:
      return menuManager.menuInputPreview(state.selectInput.getCurrentOption());

    case InputActionType::LCD_BRIGHTNESS_SETTING:
      return lcdBrightnessPreview(state.rangeInput.getValue());
    
    case InputActionType::MATRIX_BRIGHTNESS_SETTING:
      return matrixBrightnessPreview(state.rangeInput.getValue());

    case InputActionType::SOUND_SETTING:
      return soundSettingPreview(state.selectInput.getCurrentOption());

    case InputActionType::LEADERBOARD_VIEW:
      return leaderboardPreview(state.selectInput.getCurrentOption());

    case InputActionType::HIGHSCORE_NAME:
      return;
  }
}

void InputAction::action(const InputState &state) {
  switch ((InputActionType) type) {
    case InputActionType::MENU_INPUT:
      return menuManager.menuInputAction(state.selectInput.getCurrentOption());

    case InputActionType::LCD_BRIGHTNESS_SETTING:
      return lcdBrightnessAction(state.rangeInput.getValue());
    
    case InputActionType::MATRIX_BRIGHTNESS_SETTING:
      return matrixBrightnessAction(state.rangeInput.getValue());

    case InputActionType::SOUND_SETTING:
      return soundSettingAction(state.selectInput.getCurrentOption());

    case InputActionType::HIGHSCORE_NAME:
      return highscoreNameAction(state.textInput.getInput());

    case InputActionType::LEADERBOARD_VIEW:
      return;
  }
}

bool InputAction::inputReturn(const InputState &state) {
  switch((InputActionType) type) {
    case InputActionType::LEADERBOARD_VIEW:
      return leaderboardClose(state.selectInput.currentOption);

    case InputActionType::MENU_INPUT:
      return false;

    case InputActionType::LCD_BRIGHTNESS_SETTING:
    case InputActionType::MATRIX_BRIGHTNESS_SETTING:
    case InputActionType::SOUND_SETTING:
    case InputActionType::HIGHSCORE_NAME:
      return true;
  }

  return true;
}


void InputAction::lcdBrightnessPreview(byte step) {
  statusDisp.setBrightness(step);
}

void InputAction::lcdBrightnessAction(byte step) {
  statusDisp.setBrightness(step, true);
}

void InputAction::matrixBrightnessPreview(byte step) {
  gameDisp.setBrightness(step);
}

void InputAction::matrixBrightnessAction(byte step) {
  gameDisp.setBrightness(step, true);
}

void InputAction::soundSettingPreview(byte option) {
  statusDisp.printMenuOption(option == 0 ? "Off" : "On");
}

void InputAction::soundSettingAction(byte option) {
  soundManager.setEnabled((bool) option);
}

void InputAction::leaderboardPreview(byte option) {
  if (option == leaderboardSize) {
    gameDisp.displayImage(backImage);
    statusDisp.printMenuOption("Back");
  } else {
    gameDisp.displayImage(leaderboardImage);
    const HighscoreInfo& highscore = leaderboardManager.getHighscore(option);

    if (highscore.points == maxHighscorePoints) {
      statusDisp.printLeaderboard(option + 1, 0, "-----");
    } else {
      statusDisp.printLeaderboard(option + 1, highscore.points, highscore.name);
    }
  }
}

bool InputAction::leaderboardClose(byte option) {
  return option == leaderboardSize;
}

void InputAction::highscoreNameAction(const char* input) {
  leaderboardManager.setName(input);
}
