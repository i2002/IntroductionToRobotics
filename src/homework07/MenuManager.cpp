#include "MenuManager.h"
#include <Arduino.h>
#include "context.h"


void MenuManager::resetMenu(const Menu &menu) {
  stackSize = 0;
  pushMenu(menu);
}

void MenuManager::pushMenu(const Menu &menu) {
  menuStack[stackSize].menu = &menu;
  menuStack[stackSize].savedPos = 0;
  stackSize++;
  showMenu();
}

void MenuManager::popMenu() {
  if (empty()) {
    return;
  }

  menuStack[stackSize - 1].menu = nullptr;
  menuStack[stackSize - 1].savedPos = 0;
  stackSize--;
  showMenu();
}

void MenuManager::showMenu() {
  if (!empty()) {
    inputManager.setupInput(InputActionType::MENU_INPUT);
  }
}

void MenuManager::menuInputSetup() {
  if (!empty()) {
    inputManager.setupSelectInput(currentMenu().menu->name, currentMenu().menu->lenOptions, currentMenu().savedPos);
  }
}

void MenuManager::menuInputAction(byte option) {
  if (!empty()) {
    currentMenu().savedPos = option;
    getOption(option).action.handleMenuAction();
  }
}

void MenuManager::menuInputPreview(byte option) {
  if (!empty()) {
    gameDisp.displayImage(getOption(option).image);
    statusDisp.printMenuOption(getOption(option).name);
  }
}

bool MenuManager::empty() {
  return stackSize == 0;
}

MenuStackItem& MenuManager::currentMenu() {
  return menuStack[stackSize - 1];
}

const MenuOption& MenuManager::getOption(byte index) {
  return currentMenu().menu->options[index];
}
