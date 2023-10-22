# Introduction to Robotics (2023 - 2024)

This repository contains laboratory homeworks for the Introduction to Robotics course taken at Faculty of Mathematics and Computer Science, University of Bucharest (1st semester, 3rd year).

The objective of the course is to learn to program a microcontroller and interface it with with hardware inputs and outputs in order to create functional and meaningful implementations of the core concepts.

The microcontroller used for the homeworks is an [Arduino Uno R3](https://docs.arduino.cc/hardware/uno-rev3). The code is written using [Arduino IDE](https://docs.arduino.cc/software/ide-v2) and the circuit schematics are created using [Circuit Diagram](https://www.circuit-diagram.org/).

Each homework includes all the code and schematics used in solving the homework, as well as code documentation and pictures and videos showcasing the final result.

[TOC]

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

The value pin of the potentiometers are connected to analog inputs on the Arduino board (`A0` for red, `A1` for green `A2` for blue). The cathodes of the _RGB LED_ are connected to the _PWM_ pins 3, 5 and 6 on the board. The LED connections require a resistor. The input for each potentiometer is connected to _5V_ and all components are connected to ground.

### Code description

The code can be found in `src/homework01/`. Constant definitions can be found at the beginning of the file (such as the pin numbers for the inputs and outputs and maximum values for the analog input and output).

The `setup()` function initializes the digital pins in output mode and setups the serial communication (for debugging messages).

The `loop()` function reads the input values (one for each color: red, green, blue) from the potentiometer and outputs them to the LED output pins. Because the inputs are in range `[0, 1023]` and the output must be in range `[0, 255]`, I've used the `map()` function to map the input value to the corect range for the output value.

Because the value readings were flickering quite a bit, I've used the average reading over a number of measurements in order to compute the LED output intensity. There are constants defined which set how many measurements to take into account for the average, and the delay between measurements.

### Implementation demo

_TODO: link yt_
