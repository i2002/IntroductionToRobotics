#include "MenuActionHandlers.h"
#include "context.h"
#include "resources/menuDefinitions.h"

void changeMenuHandler(byte actionData) {
  menuManager.pushMenu(menus[actionData]);
}

void menuBackHandler(byte) {
  menuManager.popMenu();
}

void startGameHandler(byte) {
  changeState(AppState::GAME_RUNNING);
}


typedef void (*ActionHandler)(byte);

ActionHandler getActionHandler(MenuAction action) {
  switch(action) {
    case MenuAction::CHANGE_MENU:
      return changeMenuHandler;
    case MenuAction::MENU_BACK:
      return menuBackHandler;
    case MenuAction::START_GAME:
      return startGameHandler;
    case MenuAction::NO_ACTION:
      return nullptr;
  }

  return nullptr;
}

void handleMenuAction(MenuAction action, byte actionData) {
  ActionHandler handler = getActionHandler(action);
  if (handler) {
    handler(actionData);
  }
}
