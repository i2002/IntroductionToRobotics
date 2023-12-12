#ifndef GAME_DISPLAY_H
#define GAME_DISPLAY_H

#include <Arduino.h>
#include "LedControl.h"
#include "GameDisplayAnimation.h"
#include "config.h"


// Custom type definitions
typedef uint64_t MatrixImage;


/**
 * @brief Control the LED matrix display.
 */
class GameDisplay {
  // Display configuration
  static const unsigned long playerBlinkInterval = 1000;
  static const unsigned long bombBlinkInterval = 200;

  // LED control state
  LedControl lc;

  // Game LED blink states
  bool playerBlinkState = false;
  bool bombBlinkState = false;
  unsigned long lastPlayerBlink = 0;
  unsigned long lastBombBlink = 0;
  unsigned long animationRender = 0;

  // Animations
  GameDisplayAnimation animation = AnimationType::NO_ANIMATION;

public:
  static const int matrixSize = 8;

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
   * @brief Update the display brightness.
   * 
   * @param brightness the display brightness (0 ... 15)
   * @param save whether to save the brightness to EEPROM or not
   */
  void setBrightness(byte brightness, bool save = false);

  /**
   * @brief Get current display brightness.
   * 
   * @return byte the current display brightness
   */
  byte getBrightness();

  /**
   * @brief Render the game state to matrix display.
   */
  void renderGame();

  /**
   * @brief Renders the next frame if animation active.
   */
  void renderAnimation();

  /**
   * @brief Display static image.
   * 
   * @param image 
   */
  void displayImage(MatrixImage image);

  /**
   * @brief Start displaying an animation.
   * 
   * @param animationType the animaton to be displayed
   * @param sync if the animation should be run in synchronous mode (disables input)
   */
  void displayAnimation(AnimationType animationType, bool sync = false);

  /**
   * @brief Get if the matrix cell state based on game state.
   *
   * @return true if LED on, false otherwise
   */
  bool gameCellState(int row, int col);

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
