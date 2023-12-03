#include "../menu.h"
#include "matrixImages.h"

// - Macro for getting the size of a statically allocated vector of MenuOptions
#define MENU_OPTIONS_SIZE(options) sizeof(options) / sizeof(MenuOption)

enum class MenuAction {
  CHANGE_MENU,
  MENU_BACK,
  START_GAME,
  NO_ACTION
};

enum class AppMenu {
  MAIN_MENU,
  SETTINGS_SUBMENU
};

const MenuOption mainMenuOptions[] = {
  { "Start game", startGameImage, MenuAction::START_GAME, 0 },
  { "Tutorial", tutorialImage, MenuAction::NO_ACTION, 0 },
  { "Settings", settingsImage, MenuAction::CHANGE_MENU, AppMenu::SETTINGS_SUBMENU },
  { "About", aboutImage, MenuAction::NO_ACTION, 0 }
};

const MenuOption submenuOptions[] = {
  { "Hello", aboutImage, MenuAction::NO_ACTION, 0 },
  { "There", aboutImage, MenuAction::NO_ACTION, 0 },
  { "Back", aboutImage, MenuAction::MENU_BACK }
};

const Menu menus[] = {
  { "Main menu", mainMenuOptions, MENU_OPTIONS_SIZE(mainMenuOptions) },
  { "Submenu", submenuOptions, MENU_OPTIONS_SIZE(submenuOptions) }
};

inline const Menu& getMenu(AppMenu menu) {
  return menus[(int) menu];
}
