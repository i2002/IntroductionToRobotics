#ifndef INPUT_HANDLERS_H
#define INPUT_HANDLERS_H

enum class InputType {
  LCD_BRIGHTNESS_SETTING,
  MATRIX_BRIGHTNESS_SETTING,
  SOUND_SETTING,
  LEADERBOARD_VIEW
};

void setupInput(InputType type);

#endif // INPUT_HANDLERS_H
