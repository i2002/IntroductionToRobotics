# Introduction to Robotics (2023 - 2024)

This repository contains laboratory homeworks for the Introduction to Robotics course taken at Faculty of Mathematics and Computer Science, University of Bucharest (1st semester, 3rd year).

The objective of the course is to learn to program a microcontroller and interface it with with hardware inputs and outputs in order to create functional and meaningful implementations of the core concepts.

The microcontroller used for the homeworks is an [Arduino Uno R3](https://docs.arduino.cc/hardware/uno-rev3). The code is written using [Arduino IDE](https://docs.arduino.cc/software/ide-v2) and the circuit schematics are created using [Circuit Diagram](https://www.circuit-diagram.org/).

Each homework includes all the code and schematics used in solving the homework, as well as code documentation and pictures and videos showcasing the final result.


## Contents

- [Homework 1: RGB LED](#homework-1-rgb-led)
- [Homework 2: Elevator simulator](#homework-2-elevator-simulator-wannabe)
- [Homework 3: 7 segment display drawing](#homework-3-7-segment-display-drawing)
- [Homework 4: Stopwatch timer](#homework-4-stopwatch-timer)


## Homework 1: RGB LED

### Task description

The objective of this homework is to digitally control the red, green and blue light intensities of a RGB LED using 3 potentiometers.

### Components used

- Arduino UNO
- 3 potentiometers
- 1 RGB LED
- 3 220 ohm resistors
- connection wires.

### Circuit diagram

The diagram of the circuit is the following:

![Homework 01 circuit diagram](doc/homework01/circuit_diagram.png)

The value pin of the potentiometers are connected to analog inputs on the Arduino board (`A0` for red, `A1` for green `A2` for blue). The anodes of the _RGB LED_ are connected to the _PWM_ pins 3, 5 and 6 on the board. The LED connections require a resistor. The input for each potentiometer is connected to _5V_ and all components are connected to ground.

### Code description

The code can be found in `src/homework01/`. Constant definitions can be found at the beginning of the file (such as the pin numbers for the inputs and outputs and maximum values for the analog input and output).

The `setup()` function initializes the digital pins in output mode and setups the serial communication (for debugging messages).

The `loop()` function reads the input values (one for each color: red, green, blue) from the potentiometer and outputs them to the LED output pins. Because the inputs are in range `[0, 1023]` and the output must be in range `[0, 255]`, I've used the `map()` function to map the input value to the corect range for the output value.

Because the value readings were flickering quite a bit, I've used the average reading over a number of measurements in order to compute the LED output intensity. There are constants defined which set how many measurements to take into account for the average, and the delay between measurements.

### Implementation demo
**Circuit implementation**

![Homework 01 circuit implementation](doc/homework01/circuit_implementation.jpg)

**Video demo**

[https://www.youtube.com/watch?v=vWhn9H-_sig](https://www.youtube.com/watch?v=vWhn9H-_sig)


## Homework 2: Elevator simulator wannabe

### Task description

The objective of the homework is to simulate the state behaviour of an elevator, using buttons to call the elevator to a floor and status LEDs and a buzzer to indicate elevator state. The elevator can be one of the states: Idle, Moving, Doors closing or Doors opening. The status LED blinks when the elevator is in movement and the floor indicator LEDs change state according to the current floor. A specific sound is played when the doors close, open or the elevator reaches a new floor.

### Components used

- Arduino UNO
- 3 push buttons
- 4 LEDs
- 4 220ohm resistors and 1 100ohm resistor
- buzzer
- connection wires

### Circuit diagram
![Homework 02 circuit diagram](doc/homework02/circuit_diagram.png)

### Code description

The code can be found in `src/homework02/`. Constant definitions can be found at the beginning of the file (such as the pin numbers for the inputs and outputs, action timings, the tone sequences). There are also defined variables for keeping program state and debounce timings. The button and LED pins and states are defined using arrays, which makes it easy to extend the program for an elevator with more floors. Tone sequences are defined as arrays of Notes (which include the frequency and duration of the tone).

The `setup()` function initializes pins and sets the first floor indicator as on.

The `loop()` function implements a state machine for the elevator, keeping track of the current state and state transitions. In order to make the code simpler, I've split the functionality in multiple functions.

The `delayedExec()` and `elevatorButtonPressed()` helper functions contain the logic for executing code only when a predefined interval elapsed using `millis()` and storing the previous `millis()` into a variable. Essentially they are replacements for using `delay()` without blocking the main loop.

The `callElevator()`, `elevatorStartMovement()` and `elevatorMovement()` action function define the behaviour when an action hapens. Call elevator is called on button press and starts the elevator movement if the right conditions are met. `elevatorStartMovement()` starts the movement transition, first waiting for the doors to close. `elevatorMovement()` handles the actions to be taken when the elevator reaches a new floor.

The `updateElevatorStatusIndicator()` functions handles status LED blinking when the elevator is moving.

The `playToneSequence()` and `asyncToneSequence()` functions handle tone sequence playing without delays. The array of notes is stored into a pointer and using `millis()` the next note is played only when the current note duration has elapsed.


### Implementation demo
**Circuit implementation**
![Homework 02 circuit implementation](doc/homework02/circuit_implementation.jpg)

**Video demo**
[https://www.youtube.com/watch?v=ePu1kwLA3go](https://www.youtube.com/watch?v=ePu1kwLA3go)


## Homework 3: 7 segment display drawing

### Task description

The objective of the homework is to control the state of a 7 segment digit display using joystick movement and switch press inputs. Additionally, I've added a buzzer for sound feedback of the state changes.

### Components used

- Arduino UNO
- a 7 segment digit display
- joystick
- buzzer
- 8 220ohm resistors and 1 100ohm resistor
- connection wires

### Circuit diagram
![Homework 03 circuit diagram](doc/homework03/circuit_diagram.png)

### Code description

The code can be found in `src/homework03/`. Constant definitions can be found at the beginning of the file (such as the pin numbers for the inputs and outputs, action timings, the tone sequences, and common annode / cathode configuration for 7 segment display). There are also defined variables for keeping program state and debounce timings. The 7 segment display pins and states are defined using arrays. Tone sequences are defined as arrays of Notes (which include the frequency and duration of the tone).

The `setup()` function initializes pins and registers the joystick switch state change interrupt handler. Because I want to catch both _pressed_ and _released_ states, I set the interrupt mode to `CHANGING`.

The `loop()` function processes input changes and triggers the respective actions. After that the display is updated to reflect the state changes. In order to make the code simpler, I've split the functionality in multiple functions.

The `resetAction()`, `toggleSegmentAction()` and `changeCurrentSegmentAction()` action handlers contain the logic of processing their respective actions. `resetAction()` resets the display and sets the current active segment to _DP_, `toggleSegmentAction()` flips the state of the active segment and `changeCurrentSegmentAction()` applies the transition matrix to determine the next active segment. All actions have an associated tone sequence that is played for sound feedback.

The `handleButtonInterrupt()` ISR function handles joystick switch debouncing using interrupts. When a state is changed and remains stable over the debounce period, the `buttonState` volatile variable is updated to reflect the state change. After the main loop reads the changed value it sets it to `UNCHANGED` so that it knows that the state change has been processed.

The `processButtonAction()` and `processJoystickMovement()` input handlers process the state and input value changes and return the action that is triggered by current state conditions. For the button long press, a timer is started on button press that either is stopped if button is released before reaching the configured long press interval or triggers the long press action when the timer exceeds the long press interval. For joystick movement, only changes from NEUTRAL to either direction or joystick return to NEUTRAL trigger a state change action.

The `updateDisplayState()` function keeps the display in sync with the state in memory. `blinkActiveSegment()` toggles the active state at a configured time interval and `resetBlinkActiveSegment()` resets the blink state (used when an action has been triggered).

`delayedExec()` is a helper function for asynchronous delayed code executions, implemented as in the previous homework. `playToneSequence()` and `asyncToneSequence()` are used for asynchronous tone sequence playing and are implemented as in the previous homework.


### Implementation demo
**Circuit implementation**
![Homework 03 circuit implementation](doc/homework03/circuit_implementation.jpg)

**Video demo**
[https://www.youtube.com/watch?v=_RZdfk5xklA](https://www.youtube.com/watch?v=_RZdfk5xklA)


## Homework 4: Stopwatch timer

### Task description

The objective of the homework is to control the state of a 7 segment digit display using joystick movement and switch press inputs. Additionally, I've added a buzzer for sound feedback of the state changes.

### Components used

- Arduino UNO
- a 4 digit 7 segment digit display
- 3 button switches
- 8 220ohm resistors
- connection wires

### Circuit diagram
![Homework 04 circuit diagram](doc/homework04/circuit_diagram.png)

### Code description

The code can be found in `src/homework04/`. Constant definitions can be found at the beginning of the file (such as the pin numbers for shift register outputs, action button inputs and display control pins, action timings, common annode / cathode configuration for 7 segment display and the max length of the lap vector). There are also defined variables for keeping program state and button press debounce timings. The stopwatch can be in one of these states: _STOPPED_, _RUNNING_, _PAUSED_. In _STOPPED_ state, the stopwatch can be started (state changing to _RUNNING_), or cycle through saved laps. In _RUNNING_ state, the count is incremented, current count can be saved as laps and the count can be paused (state changing to _PAUSED_). In _PAUSED_ state the count can be reseted (state changing to _STOPPED_) or the count can be resumed (state changing to _RUNNING_). While in _PAUSED_ state, the display blinks.

The `setup()` function initializes pins and registers the start / pause and lap button interrupt handlers triggered on `FALLING` pin state.

The `loop()` function processes input changes, increments the count, updates the blink state and updates the display to reflet state changes. In order to make the code simpler, I've split the functionality in multiple functions.

The `handleStartPauseButtonInterrupt()`, `handleLapButtonInterrupt()` and `handleResetButtonPress()` contain button press handling logic calling the respective action handlers depending on current state. The first two are interrupt handlers in order to achieve better time precision.

The `toggleStartPauseStopwatch()` action handler toggles between the _RUNNING_ and _PAUSED_ stopwatch states.

The `saveLapAction()` action handler saves the current count as a new lap. The `nextLapInsertIndex` variable is used to cycle around the index where the lap time is saved when the maximum length of the lap vector is exceeded.

The `cycleViewLapsAction()` action handler updates the `activeViewLapIndex` state to cycle around saved values. The first index provided corresponds to the oldest added value. When the index reaches the end of the lap vector, it cycles around to the beginning.

The `resetCountAction()` action handler switches to _STOPPED_ state and resets the count and the active view lap index.

The `resetLapsAction()` action handler resets the recorded laps.

The `displayNumber()` function along with `writeReg()` and `activateDisplay()` helper functions write a number to the 4 digit 7 segment display, using the shift register. `writeReg()` writes a binary encoding of the display pins (A, B, C, D, E, F, G, DP) to the register for the active display. Using `activateDisplay()`, all the 4 digits are written sequentially but fast enough so that they apear to be always on.

The `buttonInterruptDebounce()` and `buttonDebounce()` helper functions are used to identify debounced button presses inside interrupt handler or in the `loop()` function. They only return true when the debounced button state changed to pressed. The `delayedExec()` helper function handles the asyncrhonous delayed execution of code, returning true only when the specified interval has elapsed since last execution.


### Implementation demo
**Circuit implementation**
![Homework 04 circuit implementation](doc/homework04/circuit_implementation.jpg)

**Video demo**
[https://www.youtube.com/watch?v=vgNZHsnJdfU](https://www.youtube.com/watch?v=vgNZHsnJdfU)
