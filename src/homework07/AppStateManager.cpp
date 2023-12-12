#include "AppStateManager.h"
#include "context.h"
#include "utils.h"
#include "resources/menuDefinitions.h"
#include "resources/matrixImages.h"
#include "resources/displayScreens.h"
#include "resources/sounds.h"

void AppStateManager::changeState(AppState newState) {
  if (newState == AppState::UNCHANGED) {
    return;
  }

  currentState = (byte) newState;

  switch (newState) {
    case AppState::UNCHANGED:
      break;
  
    case AppState::STARTUP:
      statusDisp.printScreen(welcomeScreen);
      gameDisp.displayAnimation(AnimationType::STARTUP_ANIMATION);
      soundManager.playSound(SoundType::STARTUP_SOUND);
      setTransitionTimer(2000);
      break;
    case AppState::MAIN_NAVIGATION:
      menuManager.resetMenu(getMenu(AppMenu::MAIN_MENU));
      setInputContext(AppInputContext::UI_INPUT);
      break;

    case AppState::ABOUT:
      statusDisp.printScreen(aboutScreen);
      setInputContext(AppInputContext::SKIP_INPUT);
      break;

    case AppState::GAME_RUNNING:
      game.startGame();
      setInputContext(AppInputContext::GAME_INPUT);
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
      setInputContext(AppInputContext::SKIP_INPUT);
      break;

    case AppState::SAVE_HIGHSCORE:
      leaderboardManager.setPoints(game.getPoints());
      inputManager.setupInput(InputActionType::HIGHSCORE_NAME);
      setInputContext(AppInputContext::UI_INPUT);
      break;

    case AppState::ENDED:
      setTransitionTimer(1);
      break;
  }
}

void AppStateManager::stateTransition() {
  AppState newState = AppState::UNCHANGED;

  switch ((AppState) currentState) {
    case AppState::UNCHANGED:
      newState = AppState::STARTUP;
      break;

    case AppState::STARTUP:
      newState = AppState::MAIN_NAVIGATION;
      break;

    case AppState::MAIN_NAVIGATION:
      menuManager.showMenu();
      break;

    case AppState::ABOUT:
      newState = AppState::MAIN_NAVIGATION;
      break;

    case AppState::GAME_RUNNING:
      newState = AppState::GAME_RUNNING;
      break;

    case AppState::SCORE_REVIEW:
      newState = game.getState() == GameState::WON && leaderboardManager.isHighscore(game.getPoints()) ? AppState::SAVE_HIGHSCORE : AppState::ENDED;
      break;

    case AppState::SAVE_HIGHSCORE:
      leaderboardManager.saveHighscore();
      newState = AppState::ENDED;
      break;

    case AppState::ENDED:
      newState = AppState::MAIN_NAVIGATION;
      break;
  }

  changeState(newState);
}

void AppStateManager::tickTimer() {
  if (transitionTimerDuration != 0 && delayedExec(transitionTimer, transitionTimerDuration)) {
    transitionTimerDuration = 0;
    stateTransition();
  }
}

AppInputContext AppStateManager::getInputContext() {
  return (AppInputContext) inputContext;
}

void AppStateManager::setInputContext(AppInputContext newContext) {
  inputContext = (byte) newContext;
}

void AppStateManager::setTransitionTimer(unsigned long timerDuration) {
  setInputContext(AppInputContext::NONE);
  transitionTimer = millis();
  transitionTimerDuration = timerDuration;
}
