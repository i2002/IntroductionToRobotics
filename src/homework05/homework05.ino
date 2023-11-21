#include <EEPROM.h>

// --- Configs ---
const int maxNestedMenus = 3;
const byte ultrasonicTrigPin = 10;
const byte ultrasonicEchoPin = 9;
const byte ldrPin = A0;
const byte redLedPin = 2;
const byte greenLedPin = 3;
const byte blueLedPin = 4;

const int samplingIntervalStoreIndex = 10;
const int ultrasonicThresholdStoreIndex = samplingIntervalStoreIndex + sizeof(byte);
const int ldrThresholdStoreIndex = ultrasonicThresholdStoreIndex + sizeof(int);
const int rgbValueStoreIndex = ldrThresholdStoreIndex + sizeof(int);


// --- Type definitions ---
enum class AppMenus {
  MAIN_MENU,
  SENSOR_SETTINGS_MENU,
  RESET_LOGGER_DATA_MENU,
  SYSTEM_STATUS_MENU,
  RGB_LED_CONTROL_MENU
};

enum class MenuAction {
  CHANGE_MENU,
  MENU_BACK,
  CHANGE_INPUT_CONTEXT,
  DISPLAY_LAST_READINGS,
  DISPLAY_CURRENT_SETTINGS,
  TOGGLE_LED_AUTO_MODE,
  RESET_DATA
};

enum class InputContext {
  MENU_SELECTION,
  SAMPLING_RATE,
  ULTRASONIC_THRESHOLD,
  LDR_THERSHOLD,
  RESET_CONFIRMATION,
  READINGS_DISPLAY,
  RGB_RED_VALUE,
  RGB_GREEN_VALUE,
  RGB_BLUE_VALUE
};

#define MENU_OPTIONS_SIZE(options) sizeof(options) / sizeof(MenuOption)


struct MenuOption {
  String name;
  MenuAction action;
  int actionData;

  MenuOption(String _name, MenuAction _action) : MenuOption{_name, _action, 0} {};
  MenuOption(String _name, MenuAction _action, int _actionData) : name{_name}, action{_action}, actionData{_actionData} {};
  MenuOption(String _name, MenuAction _action, AppMenus _actionData) : name{_name}, action{_action}, actionData{(int) _actionData} {};
  MenuOption(String _name, MenuAction _action, InputContext _actionData) : name{_name}, action{_action}, actionData{(int) _actionData} {};
};

struct Menu {
  String name;
  const MenuOption *options;
  int lenOptions;
};

// --- Define app menu options ---
const MenuOption mainMenuOptions[] = {
  { "Sensor settings", MenuAction::CHANGE_MENU, AppMenus::SENSOR_SETTINGS_MENU },
  { "Reset logger data", MenuAction::CHANGE_MENU, AppMenus::RESET_LOGGER_DATA_MENU },
  { "System status", MenuAction::CHANGE_MENU, AppMenus::SYSTEM_STATUS_MENU },
  { "RGB LED Control", MenuAction::CHANGE_MENU, AppMenus::RGB_LED_CONTROL_MENU }
};

const MenuOption sensorSettingsSubmenu[] = {
  { "Sensors Sampling Interval", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::SAMPLING_RATE },
  { "Ultrasonic Alert Threshold", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::ULTRASONIC_THRESHOLD },
  { "LDR Alert Threshold", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::LDR_THERSHOLD }
};

const MenuOption resetDataSubmenu[] = {
  { "Yes", MenuAction::RESET_DATA },
  { "No", MenuAction::MENU_BACK }
};

const MenuOption systemStatusSubmenu[] = {
  { "Current Sensor Readings", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::READINGS_DISPLAY },
  { "Current Sensor Settings", MenuAction::DISPLAY_CURRENT_SETTINGS },
  { "Display Logged Data", MenuAction::DISPLAY_LAST_READINGS }
};

const MenuOption rgbLedSubmenu[] = {
  { "Manual Color Control", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::RGB_BLUE_VALUE },
  { "LED: Toggle Automatic ON/OFF", MenuAction::TOGGLE_LED_AUTO_MODE },
};


// --- Define App menus (with enum for easier access) ---
const Menu menus[] = {
  { "Main menu", mainMenuOptions, MENU_OPTIONS_SIZE(mainMenuOptions) },
  { "Sensor settings", sensorSettingsSubmenu, MENU_OPTIONS_SIZE(sensorSettingsSubmenu) },
  { "Reset logger data", resetDataSubmenu, MENU_OPTIONS_SIZE(resetDataSubmenu) },
  { "System status", systemStatusSubmenu, MENU_OPTIONS_SIZE(systemStatusSubmenu) },
  { "RGB LED Control", rgbLedSubmenu, MENU_OPTIONS_SIZE(rgbLedSubmenu) },
};


// --- Define input context and messages ---
const String inputMessages[] = {
  "Select menu option", // MENU_SELECTION
  "Input sampling rate", // SAMPLING_RATE
  "Input ultrasonic threshold", // ULTRASONIC_THRESHOLD
  "Input LDR threshold value", // LDR_THERSHOLD
  "Are you sure you want to reset configuration? (Y / N)", // RESET_CONFIRMATION
  "Press any key to return to main menu", // READINGS_DISPLAY
  "Input RGB red channel value", // RGB_RED_VALUE
  "Input RGB green channel value", // RGB_GREEN_VALUE
  "Input RGB blue channel value" // RGB_BLUE_VALUE
};

const char *invalidInputMessage = "Invalid option. Please try again";


// --- App state ---
AppMenus menuStack[maxNestedMenus];
int menuNestingLevel = 0;
InputContext currentInputContext = InputContext::MENU_SELECTION;
unsigned long lastSensorRead = 0;

const int maxLoggedValues = 10;
int ultrasonicValuesLog[maxLoggedValues];
int ldrValuesLog[maxLoggedValues];

bool ledAutoMode = false;
byte samplingRate = 1;
int ultrasonicThreshold = 0;
int ldrThreshold = 0;

void processMenuAction(MenuAction action, int actionData = 0);


void setup() {
  Serial.begin(9600);

  //
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  pinMode(ldrPin, INPUT);

  // read settings from EEPROM
  EEPROM.get(samplingIntervalStoreIndex, samplingRate);
  EEPROM.get(ultrasonicThresholdStoreIndex, ultrasonicThreshold);
  EEPROM.get(ldrThresholdStoreIndex, ldrThreshold);

  // setup menus
  menuNestingLevel = 1;
  currentInputContext = InputContext::MENU_SELECTION;
  menuStack[0] = AppMenus::MAIN_MENU;
  printInputPrompt();
}

void loop() {
  // input event
  if (Serial.available() > 0) {
    processInput();
    printInputPrompt();
    // Serial.print("Current level: ");
    // Serial.print(menuNestingLevel);
    // Serial.print(" , current menu ");
    // Serial.print((int)menuStack[menuNestingLevel]);
    // Serial.print(" , current input context: ");
    // Serial.println((int)currentInputContext);
  }


  if (delayedExec(lastSensorRead, samplingRate * 1000)) {
    processSensors();
  }
}

void processInput() {
  bool inputValid = true;
  const InputContext inputContext = currentInputContext;
  switch (inputContext) {
    case InputContext::MENU_SELECTION:
      inputValid = processMenuInput();
      break;
    
    case InputContext::SAMPLING_RATE:
      inputValid = processSamplingRateInput();
      break;
    
    case InputContext::ULTRASONIC_THRESHOLD:
      inputValid = processUltrasonicThresholdInput();
      break;

    case InputContext::LDR_THERSHOLD:
      inputValid = processLDRThresholdInput();
      break;

    default:
      Serial.read();
      currentInputContext = InputContext::MENU_SELECTION;
      break;
  }

  // If input valid return to menu
  if (!inputValid) {
    Serial.println(invalidInputMessage);
  } else if (inputContext != InputContext::MENU_SELECTION) {
    currentInputContext = InputContext::MENU_SELECTION;
  }
}

void printInputPrompt() {
  if (currentInputContext == InputContext::MENU_SELECTION) {
    printCurrentMenu();
  }

  Serial.print("> ");
  Serial.println(inputMessages[(int) currentInputContext]);
}

// --- Menu handling functions ---
bool processMenuInput() {
  int option = Serial.parseInt();
  option--; // we use 0 indexed arrays
  const Menu &currentMenu = getCurrentMenu();
  // Serial.print("Input option ");
  // Serial.print(option);
  // Serial.print(", lenOptions ");
  // Serial.println(currentMenu.lenOptions);
  
  if (menuNestingLevel > 1 && option == currentMenu.lenOptions) {
    processMenuAction(MenuAction::MENU_BACK);
  } else if (option >= 0 && option < currentMenu.lenOptions) {
    processMenuAction(currentMenu.options[option].action, currentMenu.options[option].actionData);
  } else {
    return false;
  }

  return true;
}

void processMenuAction(MenuAction action, int actionData) {
  // Serial.print("action ");
  // Serial.print((int) action);
  // Serial.print(", actionData ");
  // Serial.println(actionData);

  switch (action) {
    case MenuAction::CHANGE_MENU:
      menuStack[++menuNestingLevel] = (AppMenus) actionData;
      break;

    case MenuAction::MENU_BACK:
      menuNestingLevel--;
      break;

    case MenuAction::CHANGE_INPUT_CONTEXT:
      currentInputContext = (InputContext) actionData;
      break;

    case MenuAction::RESET_DATA:
      resetLoggedData();
      break;

    case MenuAction::DISPLAY_CURRENT_SETTINGS:
      printSensorSettings();
      break;

    case MenuAction::DISPLAY_LAST_READINGS:
      printLastLoggedData();
      break;

    case MenuAction::TOGGLE_LED_AUTO_MODE:
      ledAutoMode = !ledAutoMode;
      break;
  }
}

void printCurrentMenu() {
  const Menu &menu = getCurrentMenu();
  Serial.print("\n--- ");
  Serial.print(menu.name);
  Serial.println(" ---");

  for (int i = 0; i < menu.lenOptions; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.println(menu.options[i].name);
  }

  // if not top level, back option
  if (menuNestingLevel > 1) {
    Serial.print(menu.lenOptions + 1);
    Serial.println(". Back");
  }
  Serial.println();
}

const Menu& getCurrentMenu() {
  return menus[(int) menuStack[menuNestingLevel]];
}


// --- Input handling functions ---
bool processSamplingRateInput() {
  int input = Serial.parseInt();
  if (input <= 0 || input > 10) {
    return false;
  }

  samplingRate = (byte) input;
  EEPROM.put(samplingIntervalStoreIndex, samplingRate);
  return true;
}

bool processUltrasonicThresholdInput() {
  int input = Serial.parseInt();
  if (input <= 0 || input > 2000) {
    return false;
  }

  ultrasonicThreshold = input;
  EEPROM.put(ultrasonicThresholdStoreIndex, ultrasonicThreshold);
  return true;
}

bool processLDRThresholdInput() {
  int input = Serial.parseInt();
  if (input <= 0 || input > 2000) {
    return false;
  }

  ldrThreshold = input;
  EEPROM.put(ldrThresholdStoreIndex, ldrThreshold);
  return true;
}


// --- Data printing functions ---
void printSensorValues(int ultrasonicValue, int ldrValue) {
  Serial.print("# ");
  Serial.print(ultrasonicValue);
  Serial.print(", ");
  Serial.println(ldrValue);
}

void printSensorSettings() {
  Serial.println("\n--- Sensor settings ---");
  Serial.print("- Sampling interval: ");
  Serial.println(samplingRate);
  Serial.print("- Ultrasonic sensor threshold: ");
  Serial.println(ultrasonicThreshold);
  Serial.print("- LDR sensor threshold: ");
  Serial.println(ldrThreshold);
  Serial.print("- LED auto mode: ");
  Serial.print(ledAutoMode ? "On" : "Off");
}

void printLastLoggedData() {
  Serial.println("\n--- Last logged data ---");
  for (int i = 0; i < maxLoggedValues; i++) {
    printSensorValues(ultrasonicValuesLog[i], ldrValuesLog[i]);
  }
}

// --- Data logging ---
int currentLoggingIndex = 0;
void logSensorValues(int ultrasonicValue, int ldrValue) {
  ultrasonicValuesLog[currentLoggingIndex] = ultrasonicValue;
  ldrValuesLog[currentLoggingIndex] = ldrValue;
  currentLoggingIndex = (currentLoggingIndex + 1) % maxLoggedValues;
}

void resetLoggedData() {
  for (int i = 0; i < maxLoggedValues; i++) {
    ultrasonicValuesLog[i] = 0;
    ldrValuesLog[i] = 0;
  }
  currentLoggingIndex = 0;
  Serial.println("> Logged data reseted");
}

// --- Sensor readings ---
void processSensors() {
  int ultrasonicValue = readUltrasonicValue();
  int ldrValue = readLDRValue();

  if (ledAutoMode) {
    if (ultrasonicValue < ultrasonicThreshold || ldrValue < ldrThreshold) {
      digitalWrite(greenLedPin, LOW);
      digitalWrite(redLedPin, HIGH);
    } else {
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLedPin, LOW);
    }
  }

  if (currentInputContext == InputContext::READINGS_DISPLAY) {
    printSensorValues(ultrasonicValue, ldrValue);
  }

  logSensorValues(ultrasonicValue, ldrValue);
}

int readUltrasonicValue() {
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(ultrasonicTrigPin, LOW);
  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  return duration * 0.034 / 2;
}

int readLDRValue() {
  return analogRead(ldrPin);
}

// --- Utility functions ---

/**
 * Helper function to execute a code delay without blocking the main loop.
 * Update the lastExec argument and retrun true if the ammount specified has
 * passed since the last run.
 *
 * @param lastExec the last millis() time when the action has been executed
 * @param delay the ammount in milliseconds between two executions
 * @returns true if the ammount has passed and the lastExec argument has been
 *  updated, false otherwise
 */
bool delayedExec(unsigned long &lastExec, unsigned long delay) {
  unsigned long currentMillis = millis();
  if (currentMillis - lastExec >= delay) {
    lastExec = currentMillis;
    return true;
  }

  return false;
}
