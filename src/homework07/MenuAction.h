#ifndef MENU_ACTION_H
#define MENU_ACTION_H
#include <Arduino.h>
#include "InputAction.h"

enum class AppMenu;

enum class MenuActionType {
  CHANGE_MENU,
  MENU_BACK,
  START_GAME,
  UI_INPUT,
  NO_ACTION
};

class MenuAction {
  byte type;
  byte data;

public:
  MenuAction(MenuActionType _type, byte _data = 0);
  MenuAction(AppMenu menu);
  MenuAction(InputActionType input);
  void handleMenuAction() const;

private:
  void changeMenuHandler() const;
  void menuBackHandler() const;
  void startGameHandler() const;
  void uiInputHandler() const;
};

#endif // MENU_ACTION_H
