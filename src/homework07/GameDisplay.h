#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include <Arduino.h>
#include "LedControl.h"
#include "Game.h"
#include "config.h"


/**
 * @brief 
 * 
 */
typedef uint64_t MatrixImage;


/**
 * @brief Control the LED matrix display.
 */
class GameDisplay {
  // Display configuration
  static const unsigned long playerBlinkInterval = 1000;
  static const unsigned long bombBlinkInterval = 200;
  static const int matrixSize = 8;

  // LED control state
  byte matrixBrightness = 2;
  LedControl lc;

  // game LED blink states
  bool playerBlinkState = false;
  bool bombBlinkState = false;
  unsigned long lastPlayerBlink = 0;
  unsigned long lastBombBlink = 0;

public:
  /**
   * @brief Construct a new Display Matrix object.
   *
   * @param dinPin the data pin for LED control
   * @param clockPin the clock pin for LED control
   * @param loadPin the load pin for LED control
   */
  GameDisplay() : lc{dinPin, clockPin, loadPin, 1} {}

  /**
   * @brief Initialize LED display.
   */
  void setup();

  /**
   * @brief Update matrix state to reflect current game state.
   * 
   * @param game reference to the game state
   */
  void updateGameState(const Game &game);

  /**
   * @brief Display static image.
   * 
   * @param image 
   */
  void displayImage(MatrixImage image);

  /**
   * @brief Reset the player blink state.
   */
  void resetPlayerBlink();

  /**
   * @brief Reset the bomb blink state.
   */
  void resetBombBlink();
};

#endif // GAME_DISPLAY_H
