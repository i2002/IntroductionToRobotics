#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H
#include <Arduino.h>

enum class AppState {
  UNCHANGED,
  STARTUP,
  MAIN_NAVIGATION,
  ABOUT,
  GAME_RUNNING,
  SCORE_REVIEW,
  SAVE_HIGHSCORE,
  ENDED
};

enum class AppInputContext {
  NONE,
  UI_INPUT,
  GAME_INPUT,
  SKIP_INPUT
};

class AppStateManager {
  byte currentState = (byte) AppState::UNCHANGED;
  byte inputContext = (byte) AppInputContext::NONE;
  unsigned long transitionTimer = 0;
  unsigned long transitionTimerDuration = 0;

public:
  void changeState(AppState newState);
  void stateTransition();
  void tickTimer();
  AppInputContext getInputContext();

private:
  void setInputContext(AppInputContext newContext);
  void setTransitionTimer(unsigned long timerDuration);
};

#endif // APP_STATE_MANAGER_H
