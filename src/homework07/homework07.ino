
#include "config.h"
#include "context.h"
#include "resources/matrixImages.h"
#include "resources/displayScreens.h"
#include "resources/menuDefinitions.h"


// - Input components
Joystick joystick(joystickPinX, joystickPinY, false, false);
Button triggerBtn(triggerPin);

// - Output components
StatusDisplay statusDisp;
GameDisplay gameDisp;

// - App state components
AppStateManager appStateManager;
MenuManager menuManager;
InputManager inputManager;
LeaderboardManager leaderboardManager;
Game game;


void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A2));

  joystick.setup();
  triggerBtn.setup();

  gameDisp.setup();
  statusDisp.setup();

  appStateManager.changeState(AppState::STARTUP);  
}

void loop() {
  switch(appStateManager.getInputContext()) {
    case AppInputContext::NONE:
      break;

    case AppInputContext::UI_INPUT:
      uiNavigationRuntime();
      break;

    case AppInputContext::GAME_INPUT:
      gameRuntime();
      break;

    case AppInputContext::SKIP_INPUT:
      if (triggerBtn.buttonPressed()) {
        appStateManager.stateTransition();
      }
  }

  appStateManager.tickTimer();
  gameDisp.renderAnimation();
}

inline void uiNavigationRuntime() {
  if (joystick.processMovement()) {
    inputManager.processMovement(joystick.getState());
  }

  if (triggerBtn.buttonPressed()) {
    inputManager.processActionBtn();
  }
}

inline void gameRuntime() {
  if (joystick.processMovement()) {
    if (game.playerMove(joystick.getState())) {
      gameDisp.resetPlayerBlink();
    }
  }

  if (triggerBtn.buttonPressed()) {
    game.placeBomb(millis());
    gameDisp.resetBombBlink();
  }

  bool exploded = game.bombTick(millis());
  if (exploded && game.getState() != GameState::RUNNING) {
    appStateManager.changeState(AppState::SCORE_REVIEW);
    return;
  }

  gameDisp.renderGame();
}
