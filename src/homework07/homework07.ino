
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
MenuManager menuManager;
InputManager inputManager;
Game game;

// - Data storage
DataStore<byte> lcdBrightnessStore(2);
DataStore<byte> matrixBrightnessStore(4);
DataStore<bool> soundSettingStore(6);
DataStore<byte, leaderboardSize> leaderboardStore(12);

AppState appState = AppState::MAIN_NAVIGATION;

const int startupDelay = 3000;

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A2));

  joystick.setup();
  triggerBtn.setup();

  gameDisp.setup();
  statusDisp.setup();

  changeState(AppState::STARTUP);  
}

void loop() {
  switch (appState) {
    case AppState::STARTUP:
      if (millis() > startupDelay) {
        changeState(AppState::MAIN_NAVIGATION);
      }
      break;

    case AppState::MAIN_NAVIGATION:
      uiNavigationRuntime();
      break;

    case AppState::GAME_RUNNING:
      gameRuntime();
      break;

    case AppState::SCORE_REVIEW:
      if (triggerBtn.buttonPressed()) {
        changeState(AppState::ENDED);
      }
      break;

    case AppState::ENDED:
      changeState(AppState::MAIN_NAVIGATION);
      break;
  }
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
    changeState(AppState::SCORE_REVIEW);
    return;
  }

  gameDisp.updateGameState(game);
}

void changeState(AppState newState) {
  switch (newState) {
    case AppState::STARTUP:
      statusDisp.printScreen(welcomeScreen);
      break;
    case AppState::MAIN_NAVIGATION:
      menuManager.pushMenu(getMenu(AppMenu::MAIN_MENU));
      break;

    case AppState::GAME_RUNNING:
      game.startGame();
      break;

    case AppState::SCORE_REVIEW:
      if (game.getState() == GameState::LOST) {
        gameDisp.displayImage(lostGameImage);
        statusDisp.printScreen(lostGameScreen);
      } else {
        gameDisp.displayImage(wonGameImage);
        statusDisp.printScreen(wonGameScreen);
        statusDisp.updatePoints(game.getPoints());
      }
    case AppState::ENDED:
      // changeState(AppState::STARTUP);
      break;
  }
  appState = newState;
}
