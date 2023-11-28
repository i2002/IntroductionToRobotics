#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>


/**
 * @brief Manage debounced button input
 */
class Button {
  // Debounce config
  static const unsigned long buttonDebounceInterval = 50;

  // Button config
  const byte buttonPin;

  // Button state
  int lastButtonRead = HIGH;
  int lastButtonState = HIGH;
  unsigned long lastButtonDebounce = 0;

public:
  /**
   * @brief Construct a new button state object
   * 
   * @param buttonPin the pin of the button
   */
  Button(const byte buttonPin) : buttonPin{buttonPin} {}

  /**
   * @brief Setup button input pin.
   */
  void setup();

  /**
   * @brief Checks whether the debounced state of the button is pressed
   * 
   * @return true if the button has been pressed
   * @return false otherwise
   */
  bool buttonPressed();
};

#endif // BUTTON_H
