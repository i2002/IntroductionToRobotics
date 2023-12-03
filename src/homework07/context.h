#ifndef CONTEXT_H
#define CONTEXT_H

#include "Joystick.h"
#include "Button.h"
#include "StatusDisplay.h"
#include "GameDisplay.h"
#include "MenuManager.h"
#include "InputManager.h"
#include "Game.h"

// - Input components
extern Joystick joystick;
extern Button triggerBtn;

// - Output components
extern StatusDisplay statusDisp;
extern GameDisplay gameDisp;

// - App state components
extern MenuManager menuManager;
extern InputManager inputManager;
extern Game game;

// - App state
enum class AppState {
  STARTUP,
  MAIN_NAVIGATION,
  GAME_RUNNING,
  GAME_LOST,
  GAME_WON
};

void changeState(AppState newState);


#endif // CONTEXT_H
