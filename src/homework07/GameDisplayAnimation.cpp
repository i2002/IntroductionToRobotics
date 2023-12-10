#include "GameDisplayAnimation.h"
#include "GameDisplay.h"
#include "utils.h"
#include "context.h"

GameDisplayAnimation::GameDisplayAnimation(AnimationType type) :
  type{(byte) type}, currentFrame{0}
{}

void GameDisplayAnimation::render(LedControl &lc) {
  if (!inProgress()) {
    return;
  }

  if (delayedExec(lastRender, 50)) {
    for (byte row = 0; row < GameDisplay::matrixSize; row++) {
      for (byte col = 0; col < GameDisplay::matrixSize; col++) {
        lc.setLed(0, col, row, renderFrame(currentFrame, row, col));
      }
    }
  
    currentFrame++;
  }
}

bool GameDisplayAnimation::inProgress() {
  return currentFrame < getLength();
}

byte GameDisplayAnimation::getLength() {
  switch((AnimationType) type) {
    case AnimationType::STARTUP_ANIMATION:
      return 4;
    case AnimationType::BOMB_EXPLODE_ANIMATION:
      return 8;
    case AnimationType::NO_ANIMATION:
      return 0;
  }

  return 0;
}

bool GameDisplayAnimation::renderFrame(byte frame, byte row, byte col) {
  switch((AnimationType) type) {
    case AnimationType::STARTUP_ANIMATION:
      return GameDisplayAnimation::renderStartupAnimation(frame, row, col);

    case AnimationType::BOMB_EXPLODE_ANIMATION:
      return GameDisplayAnimation::renderBombExplodeAnimation(frame, row, col);
    
    case AnimationType::NO_ANIMATION:
      return false;
  }

  return false;
}

bool GameDisplayAnimation::renderStartupAnimation(byte frame, byte row, byte col) {
  int mid = GameDisplay::matrixSize / 2;
  if ((row >= mid - 1 - frame && row <= mid + frame) && (col >= mid - 1 - frame && col <= mid + frame)) {
    return true;
  }

  return false;
}

bool GameDisplayAnimation::renderBombExplodeAnimation(byte frame, byte row, byte col) {
  Position bombPos = game.getBombPosition() - game.getViewportOffset();
  if (row != bombPos.y && col != bombPos.x) {
    return game.getCellType(Position{col, row} + game.getViewportOffset()) != CellType::EMPTY;
  }

  if (row == bombPos.y) {
    return col <= bombPos.x + frame && col >= bombPos.x - frame;
  }

  if (col == bombPos.x) {
    return row <= bombPos.y + frame && row >= bombPos.y - frame;
  }

  return false;
}
