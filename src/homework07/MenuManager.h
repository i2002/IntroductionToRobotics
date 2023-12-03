#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "Menu.h"

struct MenuStackItem {
  Menu const* menu;
  int savedPos;
};

class MenuManager {
  byte stackSize = 0;
  MenuStackItem menuStack[5];

public:
  void pushMenu(const Menu &menu);
  void popMenu();
  void resumeMenu();
  void menuPreview(byte option);
  void menuAction(byte option);
  const char *menuLabel(byte option);

private:
  bool empty();
  void setupMenuDisplay();
  MenuStackItem& getCurrentMenu();
  const MenuOption& getOption(byte index);
};

#endif // MENU_MANAGER_H
