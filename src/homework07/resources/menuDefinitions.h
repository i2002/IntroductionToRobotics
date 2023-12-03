#include "../menu.h"
#include "matrixImages.h"

// - Macro for getting the size of a statically allocated vector of MenuOptions
#define MENU_OPTIONS_SIZE(options) sizeof(options) / sizeof(MenuOption)

enum class AppMenu {
  MAIN_MENU,
  SETTINGS_SUBMENU
};

const MenuOption mainMenuOptions[] = {
  { "Start game", startGameImage, MenuActionType::START_GAME },
  { "Tutorial", tutorialImage, MenuActionType::NO_ACTION },
  { "Settings", settingsImage, AppMenu::SETTINGS_SUBMENU },
  { "Leaderboard", leaderboardImage, InputType::LEADERBOARD_VIEW },
  { "About", aboutImage, MenuActionType::NO_ACTION }
};

const MenuOption submenuOptions[] = {
  { "Brightness", brightnessImage, InputType::BRIGHTNESS_SETTING },
  { "Sounds", soundImage, InputType::SOUND_SETTING },
  { "Back", backImage, MenuActionType::MENU_BACK }
};

const Menu menus[] = {
  { "Main menu", mainMenuOptions, MENU_OPTIONS_SIZE(mainMenuOptions) },
  { "Settings", submenuOptions, MENU_OPTIONS_SIZE(submenuOptions) }
};

inline const Menu& getMenu(AppMenu menu) {
  return menus[(int) menu];
}
