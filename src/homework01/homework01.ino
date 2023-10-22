/*
  Homework01

  This program implements the task requirements of homework 01.
  It reads input values from 3 potentiometers and outputs the LED intensities for the Red,
  Green and Blue pins of a RGB LED.

  The circuit:
  * 3 potentiometers attached to analog inputs A0 (for red input), A1 (for green input),
    A2 (for blue input)
  * the 3 pins of the RGB LED attached to digital PWM pins: 3 (for red output), 5 (for
    green output) and 6 (for blue output)

  Created 22 Oct 2023
  By Tudor Butufei
  Modified 23 Oct 2023
  By Tudor Butufei

  https://github.com/i2002/IntroductionToRobotics/blob/main/src/homework01/homework01.ino
*/


// Pin definitions
const int inputRedPin = A0;
const int inputGreenPin = A1;
const int inputBluePin = A2;
const int outputRedPin = 3;
const int outputGreenPin = 5;
const int outputBluePin = 6;

// Ranges definitions
const int maxInputVal = 1023;
const int maxOutputVal = 255;

// Timing definitions
const int measurementDelay = 10;
const int measurementsAvgNr = 20;


// Setup function
void setup() {
  pinMode(outputRedPin, OUTPUT);
  pinMode(outputGreenPin, OUTPUT);
  pinMode(outputBluePin, OUTPUT);

  Serial.begin(9600);
}

// Loop function
void loop() {
  // read averaged input values (in order to reduce value flickering)
  long averageRed = 0;
  long averageGreen = 0;
  long averageBlue = 0;

  for (int i = 0; i < measurementsAvgNr; i++) {
    averageRed += analogRead(inputRedPin);
    averageGreen += analogRead(inputGreenPin);
    averageBlue += analogRead(inputBluePin);
    delay(measurementDelay);
  }

  int redValue = averageRed / measurementsAvgNr;
  int greenValue = averageGreen / measurementsAvgNr;
  int blueValue = averageBlue / measurementsAvgNr;

  // compute mapped output values
  int outputRedValue = map(redValue, 0, maxInputVal, 0, maxOutputVal);
  int outputGreenValue = map(greenValue, 0, maxInputVal, 0, maxOutputVal);
  int outputBlueValue = map(blueValue, 0, maxInputVal, 0, maxOutputVal);

  // set rgb led values
  analogWrite(outputRedPin, outputRedValue);
  analogWrite(outputGreenPin, outputGreenValue);
  analogWrite(outputBluePin, outputBlueValue);

  // debug print
  //printValues(redValue, greenValue, blueValue);
}

// Print values for debug
void printValues(int redValue, int greenValue, int blueValue) {
  Serial.print(redValue);
  Serial.print(" ");
  Serial.print(greenValue);
  Serial.print(" ");
  Serial.println(blueValue);
}
