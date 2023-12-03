#include "MenuManager.h"
#include <Arduino.h>
#include "context.h"


void menuPreviewHandler(byte option) {
  menuManager.menuPreview(option);
}

void menuActionHandler(byte option) {
  menuManager.menuAction(option);
}


void MenuManager::pushMenu(const Menu &menu) {
  menuStack[stackSize].menu = &menu;
  menuStack[stackSize].savedPos = 0;
  stackSize++;
  
  setupMenuDisplay();
}

void MenuManager::popMenu() {
  if (empty()) {
    return;
  }

  menuStack[stackSize - 1].menu = nullptr;
  menuStack[stackSize - 1].savedPos = 0;
  stackSize--;

  setupMenuDisplay();
}

void MenuManager::resumeMenu() {
  setupMenuDisplay();
}

void MenuManager::menuAction(byte option) {
  if (empty()) {
    return;
  }

  getCurrentMenu().savedPos = option;
  getOption(option).action.handleMenuAction();
}

void MenuManager::menuPreview(byte option) {
  if (empty()) {
    return;
  }

  gameDisp.displayImage(getOption(option).image);
  statusDisp.printMenuOption(getOption(option).name);
}

bool MenuManager::empty() {
  return stackSize == 0;
}

void MenuManager::setupMenuDisplay() {
  if (empty()) {
    return;
  }

  const MenuStackItem& currentMenu = getCurrentMenu();
  inputManager.setupSelectInput(currentMenu.menu->name, menuPreviewHandler, menuActionHandler, currentMenu.menu->lenOptions, currentMenu.savedPos);
}

MenuStackItem& MenuManager::getCurrentMenu() {
  return menuStack[stackSize - 1];
}

const MenuOption& MenuManager::getOption(byte index) {
  return getCurrentMenu().menu->options[index];
}
