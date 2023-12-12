#include "GameDisplay.h"
#include <EEPROM.h>
#include "utils.h"
#include "context.h"


void GameDisplay::setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, getBrightness());
  lc.clearDisplay(0);
}

void GameDisplay::setBrightness(byte value, bool save) {
  lc.setIntensity(0, value);
  
  if (save) {
    EEPROM.put(matrixBrightnessStoreIndex, value);
  }
}

byte GameDisplay::getBrightness() {
  byte brightness;
  EEPROM.get(matrixBrightnessStoreIndex, brightness);
  return brightness;
}

void GameDisplay::renderGame() {
  if (animation.inProgress()) {
    return;
  }

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, col, row, gameCellState(row, col));
    }
  }
}

void GameDisplay::renderAnimation() {
  animation.render(lc);
}

void GameDisplay::displayImage(MatrixImage image) {
  for (int i = 0; i < matrixSize; i++) {
    byte row = (image >> i * matrixSize) & 0xFF;
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, j, i, bitRead(row, j));
    }
  }
}

void GameDisplay::displayAnimation(AnimationType animationType, bool sync) {
  if (animation.inProgress()) {
    return;
  }

  animation = animationType;

  if (sync) {
    while (animation.inProgress()) {
      animation.render(lc);
    }
  }
}

bool GameDisplay::gameCellState(int row, int col) {
  Position pos = Position{col, row} + game.getViewportOffset();
  CellType cell = game.getCellType(pos);
  switch (cell) {
    case CellType::EMPTY:
      return false;
    case CellType::WALL:
      return true;
    case CellType::BOMB:
      if (delayedExec(lastBombBlink, bombBlinkInterval)) {
        bombBlinkState = !bombBlinkState;
      }
      return bombBlinkState;
    case CellType::PLAYER:
      if (delayedExec(lastPlayerBlink, playerBlinkInterval)) {
        playerBlinkState = !playerBlinkState;
      }
      return playerBlinkState;
  }

  return false;
}

void GameDisplay::resetPlayerBlink() {
  playerBlinkState = true;
  lastPlayerBlink = millis();
}

void GameDisplay::resetBombBlink() {
  bombBlinkState = false;
  lastBombBlink = millis();
}
