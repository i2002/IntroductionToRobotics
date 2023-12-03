#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include "Joystick.h"
#include "Button.h"
#include "StatusDisplay.h"
#include "GameDisplay.h"
#include "MenuManager.h"
#include "InputManager.h"
#include "Game.h"
#include "DataStore.h"

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

// - Data storage
extern DataStore<byte> lcdBrightnessStore;
extern DataStore<byte> matrixBrightnessStore;
extern DataStore<bool> soundSettingStore;
extern DataStore<byte, leaderboardSize> leaderboardStore;

// - App state
enum class AppState {
  STARTUP,
  MAIN_NAVIGATION,
  GAME_RUNNING,
  SCORE_REVIEW,
  ENDED
};

void changeState(AppState newState);


#endif // CONTEXT_H
