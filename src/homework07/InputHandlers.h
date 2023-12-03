#ifndef INPUT_HANDLERS_H
#define INPUT_HANDLERS_H

enum class InputType {
  BRIGHTNESS_SETTING,
  SOUND_SETTING,
  LEADERBOARD_VIEW
};

void setupInput(InputType type);

#endif // INPUT_HANDLERS_H
