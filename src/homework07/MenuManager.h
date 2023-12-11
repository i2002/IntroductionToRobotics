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
  void resetMenu(const Menu &menu);
  void pushMenu(const Menu &menu);
  void popMenu();
  void showMenu();
  void menuPreview(byte option);
  void menuAction(byte option);

private:
  bool empty();
  MenuStackItem& getCurrentMenu();
  const MenuOption& getOption(byte index);
};

#endif // MENU_MANAGER_H
