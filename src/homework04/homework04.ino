/*
  Homework04

  This program implements the task requirements of homework 04.
  The objective is to implement a simple stopwatch using the 4 digit 7 segment display.
  There are 3 action buttons: start / pause, reset and lap. The start / pause button toggles
  between the paused and counting modes. The reset button either resets the counter or the
  saved laps based on current state. The lap button either saves the current lap count or
  cycles through the saved laps. The display input is provided through a shift register.

  The circuit is comprised of:
  * a 4 digit 7 segment display (with segment inputs connected to the shift register)
  * 3 buttons (connected to digital pins 8, 3 and 2)
  * a SN74HC595 shift register IC (STCP connected to digital pin 11, SHCP to the digital pin 10
    and DS connected to the digital pin 12)

  Created 12 Nov 2023
  By Tudor Butufei
  Modified 12 Nov 2023
  By Tudor Butufei

  https://github.com/i2002/IntroductionToRobotics/blob/main/src/homework04/homework04.ino
*/


/*
 * Action buttons pin configuration
 */
const int startPauseButtonPin = 3;
const int lapButtonPin = 2;
const int resetButtonPin = 8;


/*
 * Display configuration
 */
// Pin definitions for interfacing with the shift register
const int latchPin = 11;  // Connects to STCP of the shift register
const int clockPin = 10;  // Connects to SHCP of the shift register
const int dataPin = 12;   // Connects to DS of the shift register

// Pin definitions for controlling the individual digits of the 7-segment display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

// Constant for the size of the shift register in bits (8 bits for a standard register)
const byte regSize = 8;

// Array to hold the pins that control the segments of each digit of the display
int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

const int displayCount = 4;      // Total number of digits in the display
const int encodingsNumber = 16;  // Total number of encodings for the hexadecimal characters (0-F)

// Array holding binary encodings for numbers and letters on a 7-segment display
const byte byteEncodings[encodingsNumber] = {
  // Encoding for segments A through G and the decimal point (DP)
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};


/*
 * Program configuration
 */

/** The maximum number of laps that can be stored */
const int maxLaps = 4;

/** The maximum value for the count */
const int countRange = 10000;

/** The number of digits used for decimals */
const int decimalPlaces = 1;

/** The interval in milliseconds between stopwatch count increments */
const unsigned long countIncrementInterval = 100;

/** The interval in milliseconds between display blink state changes */
const unsigned long displayBlinkInterval = 500;

/** The button press debounce interval in milliseconds */
const unsigned long buttonPressDebounceInterval = 100;


/*
 * Data types definitions
 */

/**
 * @enum The current stopwatch state.
 * - STOPPED: count not started, can cycle through saved laps, can reset laps
 * - RUNNING: count incrementing, can save lap
 * - PAUSED: count increment paused, can reset count
 */
enum class StopwatchState {
  STOPPED, RUNNING, PAUSED
};


/*
 * Program state
 */

/** The current application state (one of STOPPED, RUNNING, PAUSED) */
volatile StopwatchState state = StopwatchState::STOPPED;

/** The current stopwatch time count (integer form of seconds and 10th of a second) */
volatile unsigned int count = 0;

/** Lap times storage vector */
volatile unsigned int laps[maxLaps];

/** The number of currently saved laps */
volatile int lapsNr = 0;

/** The index in the laps vector where the next lap will be saved */
volatile int nextLapInsertIndex = 0;

/** The index of the active lap to be displayed in lap cycling mode (-1 meaning no lap is displayed) */
volatile int activeViewLapIndex = -1;

/** Whether the display blinking state is on (when the value is false) or off (otherwise) */
volatile bool displayBlinkState = false;


/*
 * Input states
 */

/** The last value read for the reset button */
int lastResetButtonRead = 0;


/*
 * Timing states
 */

/** The last millis() time when the count value has been incremented */
unsigned long lastIncrementTime = 0;

/** The last millis() time when the display blink state has been toggled */
unsigned long lastDisplayBlinkTime = 0;

/** The last micros() time when the last button interrupt has been triggered */
volatile unsigned long lastButtonInterruptTime = 0;

/** The last millis() time when the reset button state has changed */
unsigned long lastResetButtonDebounceTime = 0;


/**
 * Program setup (pin initialization and interrupt handlers registering).
 */
void setup() {
  // Set the shift register pins as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Initialize the digit control pins and set them to an off state
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  // Initialize button pins
  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(lapButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);

  // Attach action button interrupts
  attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), handleStartPauseButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(lapButtonPin), handleLapButtonInterrupt, FALLING);
}


/**
 * Main program loop.
 */
void loop() {
  // Handle inputs
  if (buttonDebounce(resetButtonPin, lastResetButtonRead, lastResetButtonDebounceTime)) {
    handleResetButtonPress();
  }

  // Count increment
  if (state == StopwatchState::RUNNING && delayedExec(lastIncrementTime, countIncrementInterval)) {
    count = (count + 1) % countRange;
  }

  // Display blink
  if (state == StopwatchState::PAUSED && delayedExec(lastDisplayBlinkTime, displayBlinkInterval)) {
    displayBlinkState =  !displayBlinkState;
  }

  // Update display  
  switch (state) {
    case StopwatchState::PAUSED:
      if (displayBlinkState) {
        activateDisplay(-1);
        break;
      }
    case StopwatchState::RUNNING:
      displayNumber(count);
      break;
    case StopwatchState::STOPPED:
      displayNumber(activeViewLapIndex != -1 ? laps[activeViewLapIndex] : 0);
      break;
  }
}


// ---- Input handlers ----
/**
 * Interrupt handler for start / pause button press.
 */
void handleStartPauseButtonInterrupt() {
  if (buttonInterruptDebounce(lastButtonInterruptTime)) {
    toggleStartPauseStopwatch();
  }
}


/**
 * Interrupt handler for lap button press.
 */
void handleLapButtonInterrupt() {
  if (buttonInterruptDebounce(lastButtonInterruptTime)) {
    switch (state) {
      case StopwatchState::RUNNING:
        saveLapAction();
        break;
      case StopwatchState::STOPPED:
        cycleViewLapsAction();
        break;
    }
  }
}


/**
 * Button press handler for reset button.
 */
void handleResetButtonPress() {
  switch(state) {
    case StopwatchState::PAUSED:
      resetCountAction();
      break;

    case StopwatchState::STOPPED:
      resetLapsAction();
      break;
  }
}


// ---- Action handlers ----
/**
 * Toggle the Running / Paused stopwatch state.
 * If the stopwatch is not started / paused it will go into RUNNING state.
 * If it already is in RUNNING state, it will go into PAUSED state.
 */
void toggleStartPauseStopwatch() {
  state = state != StopwatchState::RUNNING ? StopwatchState::RUNNING : StopwatchState::PAUSED;
  if (state == StopwatchState::PAUSED) {
    displayBlinkState = true;
  }
}


/**
 * Save current lap action.
 * The current count is saved into the laps vector at the current save position.
 * Then the current save position is incremented (with cycle around when the maximum
 * vector length is reached).
 */
void saveLapAction() {
  if (lapsNr < maxLaps) {
    lapsNr++;
  }
  laps[nextLapInsertIndex] = count;
  nextLapInsertIndex = (nextLapInsertIndex + 1) % maxLaps;
}


/**
 * Cycle laps view action.
 * Change the active view lap index to the next lap (the index is cycled around when]
 * reaching the maximum number of laps saved).
 * The first time the button is pressed, the view index will be the oldest lap recorded.
 */
void cycleViewLapsAction() {
  if (activeViewLapIndex == -1 && lapsNr == maxLaps) {
    activeViewLapIndex = nextLapInsertIndex;
  } else {
    activeViewLapIndex = (activeViewLapIndex + 1) % lapsNr;
  }
}


/**
 * Reset stopwatch count action.
 * The state changes into STOPPED and the count is reseted to 0.
 * The active view lap index is set to -1 to initially display 0.
 */
void resetCountAction() {
  state = StopwatchState::STOPPED;
  count = 0;
  activeViewLapIndex = -1;
}


/**
 * Reset the saved laps action.
 * The laps vector along with its length and the insert and view indexes are reseted.
 */
void resetLapsAction() {
  for (int i = 0; i < maxLaps; i++) {
    laps[i] = 0;
  }

  lapsNr = 0;
  nextLapInsertIndex = 0;
  activeViewLapIndex = -1;
}


// ---- Display functions ----
/**
 * Display a number on 7-segment digits display.
 *
 * @param number the number to be displayed
 */
void displayNumber(unsigned int number) {
  unsigned int currentNumber = number;

  for (int i = displayCount - 1; i >= 0; i--) {
    writeReg(0);
    activateDisplay(i);
    int dp = i == displayCount - decimalPlaces - 1;
    writeReg(byteEncodings[currentNumber % 10] + dp);
    currentNumber /= 10;
  }
}


/**
 * Write bits to the shift register.
 *
 * @param encoding binary representation to be written to shift register
 */
void writeReg(int encoding) {
  // Function to output a byte to the shift register
  digitalWrite(latchPin, LOW);  // Pull latch low to start data transfer
  
  // Shift out the bits of the 'encoding' to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);  // MSBFIRST means the most significant bit is shifted out first
    
  // Pull latch high to transfer data from shift register to storage register
  digitalWrite(latchPin, HIGH);  // This action updates the output of the shift register
}


/**
 * Activate a digit display.
 *
 * @param displayNumber the number of the digit to be activated (0 meaning the most left one),
 *   or -1 to turn off all digits
 */
void activateDisplay(int displayNumber) {
  // FIXME: common anode / cathode config
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }

  if (displayNumber != -1) {
    digitalWrite(displayDigits[displayNumber], LOW);
  }
}


// ---- Helper functions ----
/**
 * Helper function to achieve button debouncing in interrupt handler.
 * The function handles the debounce timing and returns true if the pressed action can
 * be executed.
 *
 * @param lastButtonInterruptTime reference to variable storing the last interrupt time
 * @returns true if the debounced state of the button is pressed, false otherwise
 */
bool buttonInterruptDebounce(volatile unsigned long &lastButtonInterruptTime) {
  unsigned long buttonInterruptTime = micros();
  bool buttonPressed = buttonInterruptTime - lastButtonInterruptTime > buttonPressDebounceInterval * 1000;
  lastButtonInterruptTime = buttonInterruptTime;
  return buttonPressed;
}


/**
 * Helper function to read button debounced state change.
 * The values of the last two arguments are updated accordingly and the function returns true
 * if the button debunced state changed to pressed.
 *
 * @param buttonPin the pin of the button value to read
 * @param lastButtonState reference to variable storing the last button read
 * @param lastButtonDebounceTime reference to variable storing the last millis() time the button
 *   read value changed
 * @returns true if the debounced state of the button changed to pressed, false otherwise
 */
bool buttonDebounce(int buttonPin, int &lastButtonRead, unsigned long &lastButtonDebounceTime) {
  bool stateChanged = false;
  int buttonValue = digitalRead(buttonPin);

  if (buttonValue != lastButtonRead) {
    stateChanged = (millis() - lastButtonDebounceTime > buttonPressDebounceInterval) && buttonValue == LOW;
    lastButtonDebounceTime = millis();
    lastButtonRead = buttonValue;
  }

  return stateChanged;
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
