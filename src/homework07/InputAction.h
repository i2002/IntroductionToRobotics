#ifndef INPUT_HANDLERS_H
#define INPUT_HANDLERS_H
#include <Arduino.h>

enum class InputActionType {
  MENU_INPUT,
  LCD_BRIGHTNESS_SETTING,
  MATRIX_BRIGHTNESS_SETTING,
  SOUND_SETTING,
  LEADERBOARD_VIEW,
  HIGHSCORE_NAME
};

union InputState;

class InputAction {
  byte type;

public:
  InputAction(InputActionType _type = InputActionType::SOUND_SETTING);

  void setupInput();

  void action(const InputState &state);

  void preview(const InputState &state);

  bool inputReturn(const InputState &state);

private:
  void lcdBrightnessPreview(byte step);
  void lcdBrightnessAction(byte step);

  void matrixBrightnessPreview(byte step);
  void matrixBrightnessAction(byte step);

  void soundSettingPreview(byte option);
  void soundSettingAction(byte option);

  void leaderboardPreview(byte option);
  bool leaderboardClose(byte option);

  void highscoreNameAction(const char* input);
};

#endif // INPUT_HANDLERS_H
