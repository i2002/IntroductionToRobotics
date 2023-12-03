#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include "GameDisplay.h"

enum class MenuAction;
enum class AppMenu;
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
  byte action;
  byte actionData;

  MenuOption(const char *_name, const MatrixImage &_image, MenuAction _action) : MenuOption{_name, _image, _action, 0} {};
  MenuOption(const char *_name, const MatrixImage &_image, MenuAction _action, AppMenu _actionData) : MenuOption{_name, _image, _action, (byte) _actionData} {};
  MenuOption(const char *_name, const MatrixImage &_image, MenuAction _action, byte _actionData) : name{_name}, image{_image}, action{(byte) _action}, actionData{_actionData} {};
  // MenuOption(const char *_name, MenuAction _action, InputContext _actionData) : name{_name}, action{_action}, actionData{(int) _actionData} {};

  MenuAction getAction() const {
    return (MenuAction) action;
  }
};


#endif // MENU_H
