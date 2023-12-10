#ifndef GAME_DISPLAY_ANIMATION_H
#define GAME_DISPLAY_ANIMATION_H
#include <Arduino.h>
#include <LedControl.h>

enum class AnimationType {
  NO_ANIMATION,
  STARTUP_ANIMATION,
  BOMB_EXPLODE_ANIMATION
};

class GameDisplayAnimation {
  byte type;
  byte currentFrame = 0;
  unsigned long lastRender = 0;

public:
  GameDisplayAnimation(AnimationType type);

  void render(LedControl &lc);

  bool inProgress();

private:
  byte getLength();
  bool renderFrame(byte frame, byte row, byte col);
  bool renderStartupAnimation(byte frame, byte row, byte col);
  bool renderBombExplodeAnimation(byte frame, byte row, byte col);
};

#endif // GAME_DISPLAY_ANIMATION_H
