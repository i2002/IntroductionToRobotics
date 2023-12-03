#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include "GameDisplay.h"
#include "MenuAction.h"

struct MenuOption;

/**
 * @struct Menu
 * This struct defines the information about a menu (its name and the options of that menu).
 */
struct Menu {
  const char *name;
  const MenuOption *options;
  int lenOptions;
};

/**
 * @struct MenuOption
 * This struct defines the information about a menu option (its name, the action triggered by the menu and the data associated with the action).
 * It is used to configure the options displayed when a menu is active.
 */
struct MenuOption {
  const char *name;
  const MatrixImage &image;
  MenuAction action;

  MenuOption(const char *_name, const MatrixImage &_image, MenuAction _action) : name{_name}, image{_image}, action{_action} {};
};

#endif // MENU_H
