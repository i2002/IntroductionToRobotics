#include "GameDisplay.h"
#include <EEPROM.h>
#include "utils.h"


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

void GameDisplay::updateGameState(const Game &game) {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      bool state = false;
      CellType cell = game.getViewportCellType({col, row});
      switch (cell) {
        case CellType::EMPTY:
          state = false;
          break;
        case CellType::WALL:
          state = true;
          break;
        case CellType::BOMB:
          if (delayedExec(lastBombBlink, bombBlinkInterval)) {
            bombBlinkState = !bombBlinkState;
          }
          state = bombBlinkState;
          break;
        case CellType::PLAYER:
          if (delayedExec(lastPlayerBlink, playerBlinkInterval)) {
            playerBlinkState = !playerBlinkState;
          }
          state = playerBlinkState;
          break;
      }

      lc.setLed(0, col, row, state);
    }
  }
}

void GameDisplay::displayImage(MatrixImage image) {
  for (int i = 0; i < matrixSize; i++) {
    byte row = (image >> i * matrixSize) & 0xFF;
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, j, i, bitRead(row, j));
    }
  }
}

void GameDisplay::resetPlayerBlink() {
  playerBlinkState = true;
  lastPlayerBlink = millis();
}

void GameDisplay::resetBombBlink() {
  bombBlinkState = false;
  lastBombBlink = millis();
}
