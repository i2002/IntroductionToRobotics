/*
  Homework03

  This program implements the task requirements of homework 03.
  The objective is to control the state of a 7 segment display using
  joystick movement and switch press.

  The circuit is comprised of:
  * a 7 segment display (connecting inputs for A, B, C, D, E, F, G, DP 
    to the digital pins 12, 10, 9, 8, 7, 6, 5 and 4 respectively)
  * a joystick (connecting VRy and VRx to analog input pins A1 and A0
    respectively and switch input to digital pin 2)
  * a buzzer (connected to the digital pin 3)

  Created 02 Nov 2023
  By Tudor Butufei
  Modified 07 Nov 2023
  By Tudor Butufei

  https://github.com/i2002/IntroductionToRobotics/blob/main/src/homework03/homework03.ino
*/


/*
 * Configuration constants
 */

/** The upper threshold value above which the program considers joystick movement change */
const int joystickUpperThreshold = 600;

/** The upper threshold value below which the program considers joystick movement change */
const int joystickLowerThreshold = 400;

/** The value interval for neutral input zone when joystick returns to neutral state
    - this helps eliminate rapid state changes around threshold values */
const int joystickReturnThreshlold = 10;

/** The time interval in milliseconds for joystick switch input debounce */
const unsigned long buttonPressDebounceInterval = 50;

/** The time interval in milliseconds above which a button press is considered a long press */
const unsigned long buttonLongPressInterval = 1500;

/** The time interval in milliseconds for active segment blinking */
const unsigned long activeSegmentBlinkInterval = 300;

/** Whether the 7 segment display is in common annode or common cathode configuration */
const bool displayCommonAnnode = false;


/**
 * Component pins
 */

/** Digital pin connected to switch output */
const int joystickPinSW = 2;

/** Analog pin connected to X output */
const int joystickPinX = A0;

/** Analog pin connected to Y output */
const int joystickPinY = A1;

/** 7-segment display pins */
const int dispPinA = 12;
const int dispPinB = 10;
const int dispPinC = 9;
const int dispPinD = 8;
const int dispPinE = 7;
const int dispPinF = 6;
const int dispPinG = 5;
const int dispPinDP = 4;

/** Board pin for state change notification buzzer */
const int buzzerPin = 3;

/** 7-segment pins array */
const int segSize = 8;
const int displaySegmentPins[segSize] = {
  dispPinA, dispPinB, dispPinC, dispPinD, dispPinE, dispPinF, dispPinG, dispPinDP
};

/** The pin value considered on - this depends on common annode or cathode configuration */
const int displayOnState = displayCommonAnnode ? LOW : HIGH;


/**
 * State types definitions
 */

/** @enum Joystick position (UNCHANGED meaning the state is the same as last read) */
enum class Direction {
  UP, DOWN, LEFT, RIGHT, NEUTRAL, UNCHANGED
};

/** @enum Button state (UNCHANGED meaning the state is the same as last read) */
enum class ButtonState {
  PRESSED, RELEASED, UNCHANGED
};

/** @enum The actions triggered by button press */
enum class ButtonAction {
  PRESS, LONG_PRESS, NONE
};

/** @enum Definitions for segment indexes */
enum DisplaySegment {
  segA, segB, segC, segD, segE, segF, segG, segDP
};

/**
 * State transition matrix: specifies which will be the next
 * active segment when moving up, down, left or right
 */
const int stateTransitionMatrix[segSize][4] = {
  // up, down, left, right  
  {segA, segG, segF, segB},    // a
  {segA, segG, segF, segB},    // b
  {segG, segD, segE, segDP},   // c
  {segG, segD, segE, segC},    // d
  {segG, segD, segE, segC},    // e
  {segA, segG, segF, segB},    // f
  {segA, segD, segG, segG},    // g
  {segDP, segDP, segC, segDP}, // dp
};


/**
 * Application state
 */

/** The logic state for each segment: true means turned on, false means turned off */
bool displaySegmentStates[segSize];

/** The physical state for the active blink: HIGH or LOW */
byte activeBlinkState = !displayOnState;

/** The index of the currently selected segment */
int currentSegmentIndex = segDP;


/**
 * Input state
 */

/** The current joystick position (UP, DOWN, LEFT, RIGHT or NEUTRAL) */
Direction joystickPosition = Direction::NEUTRAL;

/** Whether to check for long press timing conditions or not (the value is changed
    to true on button press and changed back to false when release before long press
    interval or long press action had been executed) */
bool buttonLongPressTimerActive = false;

/** Synchronize joystick switch state between interrupt handler and main loop.
    When a state change occurs, the state is updated to the new state. After
    the main loop processes the state change, the state value is set to UNCHANGED */
volatile ButtonState buttonState = ButtonState::UNCHANGED;


/**
 * Timing states
 */

/** The last millis() time when the active segment blink state had been toggled */
unsigned long lastActiveSegmentBlink = 0;

/** The last millis() time when the joystick switch has been pressed */
unsigned long buttonLongPressTimer = 0;

/** The last micros() time when interrupt handler had been called because of state
    change - used for joystick switch debouncing */
volatile unsigned long lastButtonInterruptTime = 0;


/*
 * Asynchronous tone player
 */
struct Note {
  int value;
  unsigned long duration;
};

/** Pointer to the current tone sequence playing (an array of Notes, where the last note has the value -1) */
Note *currentToneSequence = nullptr;

/** The index to the current note playing in the tone sequence */
int currentToneIndex = 0;

/** The millis() time of the last tone note played */
unsigned long lastTonePlayerNote;


/*
 * Defined tone sequences
 */
const Note stateResetTone[] = {
  {.value = 1080, .duration = 600},
  {.value = -1, .duration = 0}
};

const Note joystickMoveTone[] = {
 {.value = 698, .duration = 300},
 {.value = -1, .duration = 0}
};

const Note toggleSegmentTone[] = {
  {.value = 880, .duration = 300},
  {.value = -1, .duration = 0}
};


// --- Main program ---
/**
 * Setup.
 * Configure joystick switch pin as pull up input.
 * Configure 7 segment display and buzzer output pins.
 * Register joystick switch interrupt handler (for debouncing).
 */
void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(displaySegmentPins[i], OUTPUT);
  }

  pinMode(joystickPinSW, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(joystickPinSW), handleButtonInterrupt, CHANGE);
}


/**
 * Main loop.
 * Handle program state changes.
 */
void loop() {
  // Process input state changes
  // > Joystick switch press and long press
  switch (processButtonAction()) {
    case ButtonAction::PRESS:
      toggleSegmentAction(currentSegmentIndex);
      break;
    
    case ButtonAction::LONG_PRESS:
      resetAction();
      break;
  }

  // > Joystick movement
  if (processJoystickMovement() != Direction::UNCHANGED) {
    changeCurrentSegmentAction(joystickPosition);
  }

  // Display state changes
  updateDisplayState();

  // Process asynchronous processes
  asyncToneSequence();
}


// --- Action handlers ---
/**
 * Reset state action.
 * Resets display state and current segment and plays the reset tone sequence.
 */
void resetAction() {
  for (int i = 0; i < segSize; i++) {
    displaySegmentStates[i] = !displayOnState;
  }
  currentSegmentIndex = segDP;
  playToneSequence(stateResetTone);
}


/**
 * Toggle segment action.
 * Toggles the active segment state and plays the respective tone sequence.
 *
 * @param segmentIndex the index of the segment whose state is to be toggled.
 */
void toggleSegmentAction(int segmentIndex) {
  resetBlinkActiveSegment();
  displaySegmentStates[segmentIndex] = !displaySegmentStates[segmentIndex];
  playToneSequence(toggleSegmentTone);
}


/**
 * Change current segment action.
 * Updates the current segment based on the provided direction. If the state
 * changes a tone sequence is played.
 *
 * @param dir the direction (UP, DOWN, LEFT or RIGHT) for active state transition
 */
void changeCurrentSegmentAction(Direction dir) {
  if (dir != Direction::NEUTRAL && dir != Direction::UNCHANGED) {
    int nextState = stateTransitionMatrix[currentSegmentIndex][(int) dir];
    if (nextState != currentSegmentIndex) {
      currentSegmentIndex = nextState;
      resetBlinkActiveSegment();
      playToneSequence(joystickMoveTone);
    }
  } 
}


// --- Input handlers ---

/**
 * Button state change interrupt for debouncing.
 * Updates the debounced joystick switch state for the main loop to process the state change.
 */
void handleButtonInterrupt() {
  static unsigned long buttonReleaseInterruptTime = 0;
  buttonReleaseInterruptTime = micros();

  if (buttonReleaseInterruptTime - lastButtonInterruptTime > buttonPressDebounceInterval * 1000) {
    buttonState = digitalRead(joystickPinSW) ? ButtonState::RELEASED : ButtonState::PRESSED;
  }

  lastButtonInterruptTime = buttonReleaseInterruptTime;
}


/**
 * Process button action.
 * It checks for state changes and returns if an action has ocurred (either LONG PRESS or normal PRESS).
 * Long presses are detected when the timer started at button press becomes larger than long press interval.
 * If the button is released before that condition becomes true, the timer is reset.
 *
 * @returns the button action (either LONG_PRESS, PRESS or NONE)
 */
ButtonAction processButtonAction() {
  switch(buttonState) {
    case ButtonState::PRESSED:
      buttonState = ButtonState::UNCHANGED;
      buttonLongPressTimer = millis();
      buttonLongPressTimerActive = true;
      break;

    case ButtonState::RELEASED:
      buttonState = ButtonState::UNCHANGED;
      if (millis() - buttonLongPressTimer < buttonLongPressInterval) {
        buttonLongPressTimerActive = false;
        return ButtonAction::PRESS;
      }
      break;

    case ButtonState::UNCHANGED:
      if (buttonLongPressTimerActive && (millis() - buttonLongPressTimer > buttonLongPressInterval)) {
        buttonLongPressTimerActive = false;
        return ButtonAction::LONG_PRESS;
      }
      break;
  }

  return ButtonAction::NONE;
}


/**
 * Process joystick movement.
 * Based on current values and previous joystick position, it computes if the current joystick position changed.
 * It is considered a joystick position change if leaving from NEUTRAL to either direction or returning from any
 * direction to NEUTRAL.
 *
 * @returns either the new joystick position (UP, DOWN, LEFT, RIGHT, NEUTRAL) or UNCHANGED
 */
Direction processJoystickMovement() {
  // read joystick status
  int joystickX = analogRead(joystickPinX);
  int joystickY = analogRead(joystickPinY);
  Direction nextJoystickPosition = Direction::UNCHANGED;

  // joystick returned to neutral
  bool returnedFromUp = joystickPosition == Direction::UP && joystickY < joystickUpperThreshold - joystickReturnThreshlold;
  bool returnedFromRight = joystickPosition == Direction::RIGHT && joystickX < joystickUpperThreshold - joystickReturnThreshlold;
  bool returnedFromDown = joystickPosition == Direction::DOWN && joystickY > joystickLowerThreshold + joystickReturnThreshlold;
  bool returnedFromLeft = joystickPosition == Direction::LEFT && joystickX > joystickLowerThreshold + joystickReturnThreshlold;
  bool joystickReturned = returnedFromUp || returnedFromDown || returnedFromRight || returnedFromLeft;
  if (joystickReturned) {
    nextJoystickPosition = Direction::NEUTRAL;
  }

  // movement only from neutral position
  if (joystickPosition == Direction::NEUTRAL || joystickReturned) {
    if (joystickY > joystickUpperThreshold) {
      nextJoystickPosition = Direction::UP;
    } else if (joystickX > joystickUpperThreshold) {
      nextJoystickPosition = Direction::LEFT;
    } else if (joystickY < joystickLowerThreshold) {
      nextJoystickPosition = Direction::DOWN;
    } else if (joystickX < joystickLowerThreshold) {
      nextJoystickPosition = Direction::RIGHT;
    }
  }

  if (nextJoystickPosition != Direction::UNCHANGED) {
    joystickPosition = nextJoystickPosition;
  }

  return nextJoystickPosition;
}

// --- Output handlers ---

/**
 * Update 7 segment display state.
 */
void updateDisplayState() {
  for (int i = 0; i < segSize; i++) {
    if (i == currentSegmentIndex) {
      blinkActiveSegment();
    } else {
      digitalWrite(displaySegmentPins[i], displaySegmentStates[i] ? displayOnState : !displayOnState);
    }
  }
}


/**
 * Blink active segment state.
 */
void blinkActiveSegment() {
  if(delayedExec(lastActiveSegmentBlink, activeSegmentBlinkInterval)) {
    activeBlinkState = !activeBlinkState;
    digitalWrite(displaySegmentPins[currentSegmentIndex], activeBlinkState);
  }
}


/**
 * Reset the active segment blink state.
 * This is used when an action has ocurred.
 */
void resetBlinkActiveSegment() {
  lastActiveSegmentBlink = 0;
  activeBlinkState = displaySegmentStates[currentSegmentIndex];
}


// --- Helper functions ---
/**
 * Helper function to execute a code delay without blocking the main loop.
 * Update the lastExec argument and retrun true if the ammount specified has
 * passed since the last run.
 *
 * @param lastExec the last millis() time when the action has been executed
 * @param delay the ammount in milliseconds between two executions
 * @returns true if the ammount has passed and the lastExec argument has been
 *  updated, false otherwise
 */
bool delayedExec(unsigned long &lastExec, unsigned long delay) {
  unsigned long currentMillis = millis();
  if (currentMillis - lastExec >= delay) {
    lastExec = currentMillis;
    return true;
  }

  return false;
}


// --- Asynchronous tone sequence player ---
/**
 * Start playing a tone sequence.
 */
void playToneSequence(Note *sequence) {
  currentToneSequence = sequence;
  currentToneIndex = 0;
  tone(buzzerPin, sequence[0].value, sequence[0].duration);
  lastTonePlayerNote = millis();
}


/**
 * Continue playing the active tone sequence.
 */
void asyncToneSequence() {
  if (currentToneSequence && delayedExec(lastTonePlayerNote, currentToneSequence[currentToneIndex].duration)) {
    Note nextNote = currentToneSequence[currentToneIndex + 1];
    
    // tone sequence ended
    if (nextNote.value == -1 || nextNote.duration == 0) {
      currentToneSequence = nullptr;
      currentToneIndex = 0;
      return;
    }

    // play next tone
    tone(buzzerPin, nextNote.value, nextNote.duration);
    currentToneIndex++;
  }
}
 