#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

/** @enum Joystick position (UNCHANGED meaning the state is the same as last read) */
enum class JoystickPosition {
  UP, DOWN, LEFT, RIGHT, NEUTRAL
};


/**
 * @brief Manage joystick state.
 */
class Joystick {
  // Joystick behaviour config
  static const int joystickUpperThreshold = 650;
  static const int joystickLowerThreshold = 350;
  static const int joystickReturnThreshlold = 50;

  // Joystick config
  const byte joystickPinX;
  const byte joystickPinY;
  const bool invertX;
  const bool invertY;

  // Joystick state
  JoystickPosition currentJoystickState = JoystickPosition::NEUTRAL;

public:
  /**
   * @brief Construct a new Joystick object.
   * By default, the input value range is between 0 for upmost LEFT and UP and 1023 for upmost RIGHT and DOWN.
   * This can be configured by the invertX and invertY params depending on hardware orientation.
   * 
   * @param joystickPinX board pin for x axis input
   * @param joystickPinY board pin for y axis input
   * @param invertX if the x axis should be inverted
   * @param intertY if the y axis should be inverted
   */
  Joystick(const byte joystickPinX, const byte joystickPinY, const bool invertX = false, const bool intertY = false) : 
    joystickPinX{joystickPinX}, joystickPinY{joystickPinY}, invertX{invertX}, invertY{invertY} {}


  /**
   * @brief Setup joystick input pins.
   */
  void setup();

  /**
   * @brief Process joystick movement.
   * Based on current values and previous joystick position, it computes if the current joystick position changed.
   * It is considered a joystick position change if leaving from NEUTRAL to either direction or returning from any
   * direction to NEUTRAL.
   *
   * @return true if the joystick state changed
   * @return false otherwise
   */
  bool processMovement();

  /**
   * @brief Get current joystick state.
   *
   * @return JoystickPosition
   */
  JoystickPosition getState();

private:
  /**
   * @brief Check if the value passes the upper / lower threshold.
   * 
   * @param value joystick axis value
   * @param upper true for upper threshold, false for lower threshold
   * @param inverted whether the axis range is inverted or not
   * @return true if the joystick exceeds the threshold in the specified direction
   * @return false otherwise
   */
  bool checkThreshold(int value, bool upper);

  /**
   * @brief Check if the joystick axis value returned from the specified position
   * 
   * @param value joystick axis value
   * @param upper true for upper threshold, false for lower threshold
   * @param inverted whether the axis range is inverted or not
   * @return true if the joystick returned from the specified direction
   * @return false otherwise
   */
  bool checkReturn(int value, bool upper);

  /**
   * @brief Check if a joystick position is triggered on the upper or lower threshold of the value range.
   * 
   * @param pos the joystick position
   * @return true the specified joystick position is triggered on upper threshold
   * @return false the specified joystick position is triggered on lower threshold
   */
  bool Joystick::isUpperThreshold(JoystickPosition pos);
};

#endif // JOYSTICK_H
