#include "MenuAction.h"
#include "context.h"
#include "resources/menuDefinitions.h"

MenuAction::MenuAction(MenuActionType _type, byte _data) : type{(byte) _type}, data{_data} {}

MenuAction::MenuAction(AppMenu menu) : MenuAction(MenuActionType::CHANGE_MENU, (byte) menu) {}

MenuAction::MenuAction(InputActionType input) : MenuAction(MenuActionType::UI_INPUT, (byte) input) {}

void MenuAction::handleMenuAction() const {
  switch((MenuActionType)type) {
    case MenuActionType::CHANGE_MENU:
      return changeMenuHandler();
    case MenuActionType::MENU_BACK:
      return menuBackHandler();
    case MenuActionType::START_GAME:
      return startGameHandler();
    case MenuActionType::UI_INPUT:
      return uiInputHandler();
    case MenuActionType::NO_ACTION:
      return;
  }
}


void MenuAction::changeMenuHandler() const {
  menuManager.pushMenu(menus[data]);
}

void MenuAction::menuBackHandler() const {
  menuManager.popMenu();
}

void MenuAction::startGameHandler() const {
  appStateManager.changeState(AppState::GAME_RUNNING);
}

void MenuAction::uiInputHandler() const {
  inputManager.setupInput((InputActionType) data);
}
