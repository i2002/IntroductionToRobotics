
#include "config.h"
#include "context.h"
#include "resources/matrixImages.h"
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

AppState appState = AppState::MAIN_NAVIGATION;

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A2));

  joystick.setup();
  triggerBtn.setup();

  gameDisp.setup();
  statusDisp.setup();

  changeState(AppState::MAIN_NAVIGATION);  
}

void loop() {
  if (appState == AppState::GAME_RUNNING)   {
    gameRuntime();
  } else {
    uiNavigationRuntime();
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

  if (game.bombTick(millis())) {
    if (game.getState() == GameState::LOST) {
      changeState(AppState::GAME_LOST);
      return;
    } else if (game.getState() == GameState::WON) {
      changeState(AppState::GAME_WON);
      return;
    }
  }

  gameDisp.updateGameState(game);
}

void changeState(AppState newState) {
  switch (newState) {
    case AppState::MAIN_NAVIGATION:
      menuManager.pushMenu(getMenu(AppMenu::MAIN_MENU));
      break;

    case AppState::GAME_RUNNING:
      game.startGame();
      break;

    case AppState::GAME_LOST:
      gameDisp.displayImage(lostGameImage);
      statusDisp.printMessage("You lost :(");
      break;

    case AppState::GAME_WON:
      gameDisp.displayImage(wonGameImage);
      statusDisp.printMessage("You won :)");
      break;

    default:
      break;
  }
  appState = newState;
}
