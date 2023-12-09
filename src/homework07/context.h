#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include "Joystick.h"
#include "Button.h"
#include "StatusDisplay.h"
#include "GameDisplay.h"
#include "MenuManager.h"
#include "InputManager.h"
#include "LeaderboardManager.h"
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
extern LeaderboardManager leaderboardManager;
extern Game game;

// - App state
enum class AppState {
  STARTUP,
  MAIN_NAVIGATION,
  GAME_RUNNING,
  SCORE_REVIEW,
  SET_HIGHSCORE_NAME,
  SAVE_HIGHSCORE,
  ENDED
};

void changeState(AppState newState);


#endif // CONTEXT_H
