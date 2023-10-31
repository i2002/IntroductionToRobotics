/*
  Homework02

  This program implements the task requirements of homework 02.
  It simulates the indicator and states of a 3 floors elevator.
  There are 3 buttons that act ast call elevator buttons for each
  floor and 3 LED indicators to represent the current floor. There
  is also a fourth LED that indicates by blinking if the elevator
  is moving or not. Using a buzzer, there are specific sounds played
  when the elevator closes or opens its doors or when it reaches a
  new floor.

  The circuit is comprised of:
  * 3 buttons (connected to the digital pins 2, 3 and 4)
  * 4 leds (connected to the digital pins 5, 6, 7 and 8)
  * a buzzer (connected to the digital pin 9)

  Created 30 Oct 2023
  By Tudor Butufei
  Modified 31 Oct 2023
  By Tudor Butufei

  https://github.com/i2002/IntroductionToRobotics/blob/main/src/homework02/homework02.ino
*/


/*
 * Configuration constants
 */

/** The number of floors of the elevator */
const unsigned int numFloors = 3;

/** The time interval in mililseconds that it takes for the elevator to go from the current floor to the next */
const unsigned long floorChangeInterval = 4000;

/** The time interval in milliseconds that it takes for the elevator to close its doors */
const unsigned long doorsCloseInterval = 1000;

/** The time interval in milliseconds that it takes for the elevator to open its doors */
const unsigned long doorsOpenInterval = 1000;

/** The time interval in milliseconds between the state LED to change its blink state */
const unsigned long elevatorStatusIndicatorBlinkInterval = 500;

/** The time interval in milliseconds for debouncing floor buttons state */
const unsigned long floorButtonDebounceInterval = 100;


/*
 * Component pins
 */

/** Board pins for the floor indicator LEDs (one for each floor) */
const uint8_t floorIndicatorPins[numFloors] = {5, 6, 7};

/** Board pins for floor button inputs (one for each floor) */
const uint8_t floorButtonPins[numFloors] = {2, 3, 4};

/** Board pin for the elevator status LED */
const uint8_t elevatorStatusIndicatorPin = 8;

/** Board pin for elevator buzzer */
const uint8_t elevatorBuzzerPin = 9;


/*
 * Component states
 */

/** The states (LOW meaning pressed or HIGH meaning released) for floor buttons */
int floorButtonStates[numFloors] = { LOW };

/** The last value read for floor buttons (used for debouncing button input) */
int lastFloorButtonRead[numFloors] = { LOW };

/** The state (HIGH or LOW) for the elevator status LED */
int elevatorStatusLedState = LOW;


/*
 * Application states
 */

/** Enumeration that contains all valid elevator states */
enum class ElevatorState { IDLE, MOVING, DOORS_CLOSING, DOORS_OPENING };

/** The current state of the elevator */
ElevatorState elevatorState = ElevatorState::IDLE;

/** The current floor of the elevator (must be >= 0 and < numFloors) */
unsigned int elevatorCurrentFloor = 0;

/** The target floor of the elevator (must be >= 0 and < numFloors) */
unsigned int elevatorTargetFloor = 0;


/*
 * Timing states
 */

/** The last millis() time for each floor button when it changed it's state (used for input debouncing) */
unsigned long lastFloorButtonDebounce[numFloors];

/** The last millis() time when the elevator LED changed it's value */
unsigned long lastElevatorStatusIndicatorBlink;

/** 
 * The last millis() time when the elevator changed it's state.
 * This is used to compute the delay for the next state change
 * (when changing from doors closing to moving, when changing current floor while moving)
 */
unsigned long lastElevatorStateChange;

/** The millis() time of the last tone note played */
unsigned long lastTonePlayerNote;


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


/*
 * Defined tone sequences
 */
const Note doorsOpenTone[] = {
  {.value = 698, .duration = 300},
  {.value = 1047, .duration = 600},
  {.value = -1, .duration = 0}
};

const Note doorsCloseTone[] = {
  {.value = 1047, .duration = 300},
  {.value = 698, .duration = 600},
  {.value = -1, .duration = 0}
};

const Note elevatorMoveTone[] = {
 {.value = 880, .duration = 300},
//  {.value = 1080, .duration = 300},
 {.value = -1, .duration = 0}
};


/**
 * Setup.
 * Configure button pins as pull up inputs and the other pins as outputs
 */
void setup() {
  Serial.begin(9600);
  for (unsigned int i = 0; i < numFloors; i++) {
    pinMode(floorButtonPins[i], INPUT_PULLUP);
  }

  for (unsigned int i = 0; i < numFloors; i++) {
    pinMode(floorIndicatorPins[i], OUTPUT);
  }

  pinMode(elevatorStatusIndicatorPin, OUTPUT);
  pinMode(elevatorBuzzerPin, OUTPUT);
  digitalWrite(floorIndicatorPins[0], HIGH);
}


/**
 * Main loop.
 * Elevator state machine.
 */
void loop() {
  // Process state transitions
  switch (elevatorState) {
    case ElevatorState::MOVING:
      updateElevatorStatusIndicator();
      if (delayedExec(lastElevatorStateChange, floorChangeInterval)) {
        elevatorMovement();

        if (elevatorCurrentFloor == elevatorTargetFloor) {
          elevatorState = ElevatorState::DOORS_OPENING;
          digitalWrite(elevatorStatusIndicatorPin, HIGH);
        }
      }
      break;

    case ElevatorState::DOORS_CLOSING:
      if (delayedExec(lastElevatorStateChange, doorsCloseInterval)) {
        elevatorState = ElevatorState::MOVING;
      }
      break;

    case ElevatorState::DOORS_OPENING:
      if (delayedExec(lastElevatorStateChange, doorsOpenInterval)) {
        elevatorState = ElevatorState::IDLE;
        playToneSequence(doorsOpenTone);
      }
      break;
  }

  // Process input events
  for (unsigned int i = 0; i < numFloors; i++) {
    if (elevatorButtonPressed(i)) {
      callElevator(i);
    }
  }

  // Process async processes
  asyncToneSequence();
}


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


/**
 * Helper function to detect elevator floor button debounced press.
 * The button action is considered triggered if the debounced state of the button
 * just changed to LOW since last read (i.e. the button has been pressed since
 * last read)
 *
 * @param i the floor number of the button
 * @returns true if the button has triggered an action, false otherwise
 */
bool elevatorButtonPressed(unsigned long i) {
  int buttonValue = digitalRead(floorButtonPins[i]);
  bool buttonPressed = false;

  // reset debounce timer if value changed to quickly
  if (buttonValue != lastFloorButtonRead[i]) {
    lastFloorButtonDebounce[i] = millis();
  }
  lastFloorButtonRead[i] = buttonValue;

  // value stable in debounce interval
  if (millis() - lastFloorButtonDebounce[i] > floorButtonDebounceInterval) {
    if (buttonValue != floorButtonStates[i]) {
      floorButtonStates[i] = buttonValue;

      if (floorButtonStates[i] == LOW) {
        // signal that the action has been triggered
        buttonPressed = true;
      }
    }
  }

  return buttonPressed;
}


/**
 * Call the elevator action.
 * If the elevator is in movement the action is ignored.
 * If the elevator is idle, close the doors and start elevator movement.
 *
 * @param floor the floor to which the elevator was called
 */
void callElevator(unsigned int floor) {
  if (floor >= numFloors || elevatorState != ElevatorState::IDLE) {
    return;
  }

  elevatorTargetFloor = floor;
  if (elevatorCurrentFloor != elevatorTargetFloor) {
    elevatorStartMovement();
  }
}


/**
 * Elevator start movement action.
 * Closes the door and initializez elevator movement.
 */
void elevatorStartMovement() {
  elevatorState = ElevatorState::DOORS_CLOSING;
  lastElevatorStateChange = millis();
  playToneSequence(doorsCloseTone);
}


/** 
 * Elevator movement action.
 * Moves the currentFloor state one floor up or down in order to get closer to target.
 * It also updates the LED states and plays the elevator move tone.
 */
void elevatorMovement() {
  if (elevatorCurrentFloor == elevatorTargetFloor) {
    return;
  }

  unsigned int nextFloor = elevatorCurrentFloor + (elevatorTargetFloor > elevatorCurrentFloor ? 1 : -1);
  digitalWrite(floorIndicatorPins[elevatorCurrentFloor], LOW);
  digitalWrite(floorIndicatorPins[nextFloor], HIGH);
  elevatorCurrentFloor = nextFloor;
  playToneSequence(elevatorMoveTone);
}


/**
 * Update the elevator state indicator state (blink the LED).
 */
void updateElevatorStatusIndicator() {
  if(delayedExec(lastElevatorStatusIndicatorBlink, elevatorStatusIndicatorBlinkInterval)) {
    elevatorStatusLedState = !elevatorStatusLedState;
    digitalWrite(elevatorStatusIndicatorPin, elevatorStatusLedState);
  }
}


/**
 * Start playing a tone sequence.
 */
void playToneSequence(Note *sequence) {
  currentToneSequence = sequence;
  currentToneIndex = 0;
  tone(elevatorBuzzerPin, sequence[0].value, sequence[0].duration);
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
    tone(elevatorBuzzerPin, nextNote.value, nextNote.duration);
    currentToneIndex++;
  }
}
