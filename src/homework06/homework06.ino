/*
  Homework06

  This program implements the task requirements of homework 06.
  The objective is to implement a simple bomber-like game using the matrix display to show game state
  and the joystick to move the player around. The game matrix contains empty cells (off LEDs), walls
  (always on LEDs), the current player position (slowly blinking LED) and the bomb (rapidly blinking LED).

  The goal of the game is to destroy all the walls by placing bombs. When the bomb goes off, the walls on
  the same row or column with the bomb are destroyed. If the player stays in the range of the bomb, loses
  the game. If all walls have been destroyed, the player wins the game.

  The circuit is comprised of:
  * a 8x8 LED matrix display
  * a joystick
  * a button
  * a MAX7219 LED driver IC

  Created 26 Nov 2023
  By Tudor Butufei
  Modified 28 Nov 2023
  By Tudor Butufei

  https://github.com/i2002/IntroductionToRobotics/blob/main/src/homework06/homework06.ino
*/

#include "Button.h"
#include "Joystick.h"
#include "DisplayMatrix.h"
#include "Game.h"
#include "matrixImages.h"


// --- Configuration ---
// - Component pins
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

const byte joystickPinX = A0;
const byte joystickPinY = A1;

const byte triggerPin = 2;


// --- Application state ---
// - Input components
Joystick joystick = Joystick(joystickPinX, joystickPinY, true, true);
Button triggerBtn = Button(triggerPin);

// - Output components
DisplayMatrix dispMatrix = DisplayMatrix(dinPin, clockPin, loadPin);

// - Game state
Game game;


/**
 * @brief Initialize program.
 */
void setup() {
  randomSeed(analogRead(A2));

  dispMatrix.setup();
  joystick.setup();
  triggerBtn.setup();

  dispMatrix.displayImage(startGameImage);
}


/**
 * @brief Main program loop.
 */
void loop() {
  switch(game.getState()) {
    case GameState::RUNNING:
      if (joystick.processMovement()) {
        if (game.playerMove(joystick.getState())) {
          dispMatrix.resetPlayerBlink();
        }
      }

      if (triggerBtn.buttonPressed()) {
        game.placeBomb(millis());
        dispMatrix.resetBombBlink();
      }

      if (game.bombTick(millis())) {
        if (game.getState() == GameState::LOST) {
          dispMatrix.displayImage(lostGameImage);
          break;
        } else if (game.getState() == GameState::WON) {
          dispMatrix.displayImage(wonGameImage);
          break;
        }
      }

      dispMatrix.updateGameState(game);
      break;

    default:
      if (triggerBtn.buttonPressed()) {
        game.startGame();
      }
  }
}
