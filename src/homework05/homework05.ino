#include <EEPROM.h>

// --- Configs ---
// - Max array lengts
const int maxNestedMenus = 3;
const int maxInputBuf = 100;
const int maxLoggedValues = 10;
const int rgbHexStrLen = 7;

// - Sensor value intervals
const int minSamplingRate = 1;
const int maxSamplingRate = 10;
const int minUltrasonicValue = 1;
const int maxUltrasonicValue = 400; // 400 cm
const int minLDRValue = 1;
const int maxLDRValue = 1023;

// - Board pins
const byte ultrasonicTrigPin = 10;
const byte ultrasonicEchoPin = 9;
const byte ldrPin = A0;
const byte redLedPin = 3;
const byte greenLedPin = 5;
const byte blueLedPin = 6;

// - EEPROM store indexes
const int samplingIntervalStoreIndex = 10;
const int ultrasonicThresholdStoreIndex = samplingIntervalStoreIndex + sizeof(byte);
const int ldrThresholdStoreIndex = ultrasonicThresholdStoreIndex + sizeof(int);
const int rgbValueStoreIndex = ldrThresholdStoreIndex + sizeof(int);


// --- Type definitions ---
// - App enums
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
  TOGGLE_LED_AUTO_MODE
};

enum class InputContext {
  MENU_SELECTION,
  SAMPLING_RATE,
  ULTRASONIC_THRESHOLD,
  LDR_THERSHOLD,
  RESET_ULTRASONIC_LOGS_CONFIRM,
  RESET_LDR_LOGS_CONFIRM,
  READINGS_DISPLAY,
  RGB_LED_VALUE
};

// - Macro for getting the size of a statically allocated vector of MenuOptions
#define MENU_OPTIONS_SIZE(options) sizeof(options) / sizeof(MenuOption)


/**
 * @struct MenuOption
 * This struct defines the information about a menu option (its name, the action triggered by the menu and the data associated with the action).
 * It is used to configure the options displayed when a menu is active.
 */
struct MenuOption {
  const char *name;
  MenuAction action;
  int actionData;

  MenuOption(const char *_name, MenuAction _action) : MenuOption{_name, _action, 0} {};
  MenuOption(const char *_name, MenuAction _action, int _actionData) : name{_name}, action{_action}, actionData{_actionData} {};
  MenuOption(const char *_name, MenuAction _action, AppMenus _actionData) : name{_name}, action{_action}, actionData{(int) _actionData} {};
  MenuOption(const char *_name, MenuAction _action, InputContext _actionData) : name{_name}, action{_action}, actionData{(int) _actionData} {};
};


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
 * @struct SensorLog
 * This struct is used to log the last nth (maxLoggedValues) values recorded for a sensor.
 */
struct SensorLog {
  int dataLog[maxLoggedValues];
  int currentIndex = 0;

  /**
   * Log a new sensor value.
   */
  void logValue(int value) {
    dataLog[currentIndex] = value;
    currentIndex = (currentIndex + 1) % maxLoggedValues;
  }

  /**
   * Get a logged value at a specified index.
   *
   * @param logIndex the index of the value to be returned from the log (index 0 corresponds with the oldest value recorded.)
   * @return the requested value
   */
  int getLog(int logIndex) {
    return dataLog[(currentIndex + logIndex) % maxLoggedValues];
  }

  /**
   * Reset the saved logs
   */
  void resetLogs() {
    for (int i = 0; i < maxLoggedValues; i++) {
      dataLog[i] = 0;
    }
    currentIndex = 0;
  }
};


// --- App menu structure definition ---
/*
 * Each menu and submenu is found in the menus vector (with the AppMenus enum for easier indexing).
 * For each menu there is defined a vector of MenuOptions which contain the available options for that menu.
 * The menu routine displays the options associated with the current menu and on option input executed the action defined in the MenuOption struct.
 */
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
  { "Reset ultrasonic data", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::RESET_ULTRASONIC_LOGS_CONFIRM },
  { "Reset LDR data", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::RESET_LDR_LOGS_CONFIRM }
};

const MenuOption systemStatusSubmenu[] = {
  { "Current Sensor Readings", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::READINGS_DISPLAY },
  { "Current Sensor Settings", MenuAction::DISPLAY_CURRENT_SETTINGS },
  { "Display Logged Data", MenuAction::DISPLAY_LAST_READINGS }
};

const MenuOption rgbLedSubmenu[] = {
  { "Manual Color Control", MenuAction::CHANGE_INPUT_CONTEXT, InputContext::RGB_LED_VALUE },
  { "LED: Toggle Automatic ON/OFF", MenuAction::TOGGLE_LED_AUTO_MODE },
};


const Menu menus[] = {
  { "Main menu", mainMenuOptions, MENU_OPTIONS_SIZE(mainMenuOptions) }, // MAIN_MENU
  { "Sensor settings", sensorSettingsSubmenu, MENU_OPTIONS_SIZE(sensorSettingsSubmenu) }, // SENSOR_SETTINGS_MENU
  { "Reset logger data", resetDataSubmenu, MENU_OPTIONS_SIZE(resetDataSubmenu) }, // RESET_LOGGER_DATA_MENU
  { "System status", systemStatusSubmenu, MENU_OPTIONS_SIZE(systemStatusSubmenu) }, // SYSTEM_STATUS_MENU
  { "RGB LED Control", rgbLedSubmenu, MENU_OPTIONS_SIZE(rgbLedSubmenu) } // RGB_LED_CONTROL_MENU
};


// --- Define input prompt messages ---
/*
 * The input context is used to identify how the data from serial input should be interpreted.
 * Based on the current input context, the right handler is used to process the input and update the app state.
 * The input prompt is displayed in the console to let the user know what data does the program expect.
 * The input prompts for each input context are defined below.
 */
const char *inputPromptMessages[] = {
  "Select menu option", // MENU_SELECTION
  "Input sampling rate (between 1 and 10 seconds)", // SAMPLING_RATE
  "Input ultrasonic min alert threshold (between 1 and 400 cm)", // ULTRASONIC_THRESHOLD
  "Input LDR min alert threshold (between 1 and 1023)", // LDR_THERSHOLD
  "Are you sure you want to reset ultrasonic logged data? (y / n)", // RESET_ULTRASONIC_LOGS_CONFIRM
  "Are you sure you want to reset ldr logged data? (y / n)", // RESET_LDR_LOGS_CONFIRM
  "Press any key to return to main menu", // READINGS_DISPLAY
  "Input RGB LED VALUE (in #fffff format)" // RGB_LED_VALUE
};

const char *invalidInputMessage = "Invalid option. Please try again";


// --- App state ---
// - Menu state
/*
 * The menu works using a menu stack.
 * When selecting a menu option which leads to a submenu, the nesting level is increased and the new menu is added to the stack.
 * When selecting the back option, the last menu is popped from the stack and the nesting level is decreased.
 * The main menu cannot be removed from the stack.
 */
AppMenus menuStack[maxNestedMenus];
int menuNestingLevel = 0;

// - Serial input
/*
 * The characters received from the serial input are appended to the input buffer until either a newline character (\n or \r) is reaced
 * or the buffer reaches its maximum length.
 * When that happens, the current input context handler is executed to process the input buffer.
 * After that the process is repeated.
 */
InputContext currentInputContext = InputContext::MENU_SELECTION;
char inputBuffer[maxInputBuf];
int inputBufferIndex = 0;

// - Timing states
unsigned long lastSensorRead = 0;

// - App settings
/** The sensor sampling rate in seconds */
byte samplingRate = 1;
int ultrasonicThreshold = 0;
int ldrThreshold = 0;
long rgbValue = 0;
bool ledAutoMode = false;
bool thresholdTriggered = false;

// - Data logs
/*
 * The data for each sensor is logged in the SensorLog class which handles internally the current index and the log actions
 */
SensorLog ultrasonicValuesLog;
SensorLog ldrValuesLog;


/**
 * Process menu action.
 * The specified menu action handler is called, forwarding the action data information.
 *
 * @param action the menu action
 * @param actionData (optional) aditional data for the action handler
 */
void processMenuAction(MenuAction action, int actionData = 0);


/**
 * Program setup
 */
void setup() {
  // Setup serial
  Serial.begin(9600);

  // Setup pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  pinMode(ldrPin, INPUT);

  // Read settings from EEPROM
  EEPROM.get(samplingIntervalStoreIndex, samplingRate);
  EEPROM.get(ultrasonicThresholdStoreIndex, ultrasonicThreshold);
  EEPROM.get(ldrThresholdStoreIndex, ldrThreshold);
  EEPROM.get(rgbValueStoreIndex, rgbValue);

  // Setup menus
  menuNestingLevel = 1;
  currentInputContext = InputContext::MENU_SELECTION;
  menuStack[0] = AppMenus::MAIN_MENU;

  // Show initial prompt
  printInputPrompt();
}


/**
 * Program loop.
 * - Process input when the input is available and a complete line has been received from serial input and show the input prompt.
 * - Process sensor data at the configured sampling rate.
 */
void loop() {
  // Input event
  if (Serial.available() > 0 && receiveSerial()) {
    processInput();
    printInputPrompt();
  }

  // Sensor processing
  if (delayedExec(lastSensorRead, samplingRate * 1000)) {
    processSensors();
    updateLED();
  }
}


/**
 * Process the data from the input buffer using a handler based on the current input context.
 * If the input is invalid, the handler returns false and a warning message will be displayed.
 * On successful data input (other than menu option input), the context returns to the menu.
 */
void processInput() {
  // echo input to console
  Serial.println(inputBuffer);

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

    case InputContext::RESET_ULTRASONIC_LOGS_CONFIRM:
      inputValid = resetUltrasonicLogs();
      break;

    case InputContext::RESET_LDR_LOGS_CONFIRM:
      inputValid = resetLDRLogs();
      break;
    
    case InputContext::RGB_LED_VALUE:
      inputValid = processRgbLedValueInput();
      break;

    default:
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


/**
 * Print the current input context prompt.
 * If current context is MENU_SELECTION also print the current menu.
 */
void printInputPrompt() {
  if (currentInputContext == InputContext::MENU_SELECTION) {
    printCurrentMenu();
  }

  Serial.print("> ");
  Serial.print(inputPromptMessages[(int) currentInputContext]);
  Serial.print(": ");

  if (currentInputContext == InputContext::READINGS_DISPLAY) {
    Serial.println();
  }
}


// --- Menu handling functions ---
/**
 * Process menu input.
 * Execute the menu action associated with the selected submenu.
 *
 * @return true if selection was valid, false otherwise
 */
bool processMenuInput() {
  int option;
  if (!inputParseInt(option)) {
    return false;
  }

  option--; // we use 0 indexed arrays
  const Menu &currentMenu = getCurrentMenu();
  
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


/**
 * Displays the current menu
 */
void printCurrentMenu() {
  const Menu &menu = getCurrentMenu();
  printTitle(menu.name);

  for (int i = 0; i < menu.lenOptions; i++) {
    printMenuOption(i + 1, menu.options[i].name);
  }

  // if not top level, back option
  if (menuNestingLevel > 1) {
    printMenuOption(menu.lenOptions + 1, "Back");
  }
  Serial.println();
}


/**
 * Display format utility function to display a title
 *
 * @param text the title text
 */
void printTitle(const char* text) {
  Serial.print("\n--- ");
  Serial.print(text);
  Serial.println(" ---");
}


/**
 * Display format utility function to display a menu option
 */
void printMenuOption(int index, const char* name) {
  Serial.print(index);
  Serial.print(". ");
  Serial.println(name);
}


/**
 * Get the active menu from the menu stack.
 * @return reference to the current menu
 */
const Menu& getCurrentMenu() {
  return menus[(int) menuStack[menuNestingLevel]];
}


// --- Input handling functions ---

/**
 * Process the sampling rate input
 *
 * @return true if valid input, false otherwise
 */
bool processSamplingRateInput() {
  int input;
  bool valid = inputParseInt(input);
  if (!valid || input < minSamplingRate || input > maxSamplingRate) {
    return false;
  }

  samplingRate = (byte) input;
  EEPROM.put(samplingIntervalStoreIndex, samplingRate);
  return true;
}


/**
 * Process ultrasonic sensor threshold input.
 *
 * @return true if valid input, false otherwise
 */
bool processUltrasonicThresholdInput() {
  int input;
  bool valid = inputParseInt(input);
  if (!valid || input < minUltrasonicValue || input > maxUltrasonicValue) {
    return false;
  }

  ultrasonicThreshold = input;
  EEPROM.put(ultrasonicThresholdStoreIndex, ultrasonicThreshold);
  return true;
}


/**
 * Process LDR sensor threshold input.
 *
 * @return true if valid input, false otherwise
 */
bool processLDRThresholdInput() {
  int input;
  bool valid = inputParseInt(input);
  if (!valid || input < minLDRValue || input > maxLDRValue) {
    return false;
  }

  ldrThreshold = input;
  EEPROM.put(ldrThresholdStoreIndex, ldrThreshold);
  return true;
}


/**
 * Process ultrasonic logs reset confirmation.
 *
 * @return true if valid input, false otherwise
 */
bool resetUltrasonicLogs() {
  // input length should be 1
  if (inputBuffer[1] != 0) {
    return false;
  }

  char choice = inputBuffer[0];

  // invalid choice
  if (choice != 'y' && choice != 'n') {
    return false;
  }

  // user confirmed
  if (choice == 'y') {
    Serial.println("Data reseted.");
    ultrasonicValuesLog.resetLogs();
  }

  return true;
}


/**
 * Process LDR logs reset confirmation.
 *
 * @return true if valid input, false otherwise
 */
bool resetLDRLogs() {
  // input length should be 1
  if (inputBuffer[1] != 0) {
    return false;
  }

  char choice = inputBuffer[0];

  // invalid choice
  if (choice != 'y' && choice != 'n') {
    return false;
  }

  // user confirmed
  if (choice == 'y') {
    Serial.println("Data reseted.");
    ldrValuesLog.resetLogs();
  }

  return true;
}


/**
 * Process RGB LED color input.
 * The input is a string of the hex representation of the RGB value (for example #ffffff for values 255 255 255).
 * The function checks if the input is valid and transforms it into a long variable that contains the inputed value.
 *
 * @return true if valid input, false otherwise
 */
bool processRgbLedValueInput() {
  // should start with #
  if (inputBuffer[0] != '#') {
    return false;
  }

  // should have length 7
  if (inputBuffer[rgbHexStrLen] != 0) {
    return false;
  }

  // parse hex number
  long input = 0;
  for (int i = 1; i < rgbHexStrLen; i++) {
    char digit = inputBuffer[i];
    // should contain only hex digits (0 - 9 and a - f)
    if ((digit < '0' || digit > '9') && (digit < 'a' || digit > 'f')) {
      return false;
    } else {
      int digitValue = digit <= '9' ? digit - '0' : digit - 'a' + 10;
      input = input * 16 + digitValue;
    }
  }

  // store value
  rgbValue = input;
  EEPROM.put(rgbValueStoreIndex, rgbValue);
  updateLED();
  return true;
}


// --- Data printing functions ---

/**
 * Format printing sensor values
 *
 * @param ultrasonicValue the value for the ultrasonic sensor
 * @param ldrValue the value for the LDR sensor
 */
void printSensorValues(int ultrasonicValue, int ldrValue) {
  Serial.print("- ultrasonic: ");
  Serial.print(ultrasonicValue);
  Serial.print(", ldr: ");
  Serial.println(ldrValue);
}


/**
 * Display current sensor settings.
 */
void printSensorSettings() {
  printTitle("Sensor settings");

  Serial.print("- Sampling interval: ");
  Serial.println(samplingRate);
  Serial.print("- Ultrasonic sensor threshold: ");
  Serial.println(ultrasonicThreshold);
  Serial.print("- LDR sensor threshold: ");
  Serial.println(ldrThreshold);
  Serial.print("- LED auto mode: ");
  Serial.println(ledAutoMode ? "On" : "Off");
  Serial.print("- Manual LED color: Red: ");
  Serial.print(getRedValue());
  Serial.print(", Green: ");
  Serial.print(getGreenValue());
  Serial.print(", Blue: ");
  Serial.println(getBlueValue());
}


/**
 * Print the last logged sensor values in reverse order.
 * The first printed value is the last recorded value.
 */
void printLastLoggedData() {
  printTitle("Last logged data");

  for (int i = maxLoggedValues - 1; i >= 0; i--) {
    printSensorValues(ultrasonicValuesLog.getLog(i), ldrValuesLog.getLog(i));
  }
}


// --- Serial handling ---

/**
 * Reveive chars from serial input and append them to the input buffer.
 * One input line is considered complete when either the end line character is read ('\n' or '\r') or the maximum buffer capacity is reached.
 * When a complete input line has been read, the buffer is NULL terminated and the current index is reseted.
 * It is used to know if the input processing can be triggered (i.e. a complete new input has been recorted in the input buffer)
 * 
 * @return true if the buffer contains a complete line input (that can be processed), false otherwise
 */
bool receiveSerial() {
  char recv = Serial.read();
  bool lineEndMarker = recv == '\n' || recv == '\r';

  // end of input line
  if (lineEndMarker || inputBufferIndex == maxInputBuf - 1) {
    // mark string end
    inputBuffer[inputBufferIndex] = 0;

    // ignore empty string
    if (inputBufferIndex == 0) {
      return false;
    }

    // reset input index
    inputBufferIndex = 0;

    return true;
  }

  inputBuffer[inputBufferIndex++] = recv;
  return false;
}


/**
 * Parse integer value form input buffer.
 * 
 * @param input reference to the value where to store the parsed input
 * @returns true if the parsing succeded (the buffer contains a valid integer), false otherwise
 */
bool inputParseInt(int &input) {
  input = 0;
  int negative = inputBuffer[0] == '-';

  for (int i = negative ? 1 : 0; i < maxInputBuf && inputBuffer[i] != 0; i++) {
    // character isn't a digit
    if (inputBuffer[i] < '0' || inputBuffer[i] > '9') {
      return false;
    }

    input = input * 10 + (inputBuffer[i] - '0');
  }

  // negative integer
  if (negative) {
    input *= -1;
  }

  return true;
}


// --- Sensor readings ---

/**
 * Read and log sensor values and update threshold trigger.
 */
void processSensors() {
  int ultrasonicValue = readUltrasonicValue();
  int ldrValue = readLDRValue();

  if (currentInputContext == InputContext::READINGS_DISPLAY) {
    printSensorValues(ultrasonicValue, ldrValue);
  }

  ultrasonicValuesLog.logValue(ultrasonicValue);
  ldrValuesLog.logValue(ldrValue);

  thresholdTriggered = false;
  if (ultrasonicValue < ultrasonicThreshold) {
    Serial.println("\n! Proximity alert");
    thresholdTriggered = true;
  }

  if (ldrValue < ldrThreshold) {
    Serial.println("\n! Low light conditions");
    thresholdTriggered = true;
  }
}


/**
 * Read the distance measured by the ultrasonic sensor.
 *
 * @return the distance
 */
int readUltrasonicValue() {
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(ultrasonicTrigPin, LOW);
  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  return duration * 0.034 / 2;
}


/**
 * Read the luminance measured by the LDR sensor.
 *
 * @return LDR sensor value
 */
int readLDRValue() {
  return analogRead(ldrPin);
}


// --- LED handler ---

/**
 * Update the RGB LED color based on current state.
 * - If in auto mode, red or green based on treshold trigger
 * - If in manual mode, use the values configured by user
 */
void updateLED() {
  if (ledAutoMode) {
    if (thresholdTriggered) {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
      digitalWrite(blueLedPin, LOW);
    } else {
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(blueLedPin, LOW);
    }
  } else {
    analogWrite(redLedPin, getRedValue());
    analogWrite(greenLedPin, getGreenValue());
    analogWrite(blueLedPin, getBlueValue());
  }
}


/**
 * Compute the red channel LED value from rgbValue config
 *
 * @return the specified value (0 - 255)
 */
int getRedValue() {
  return (rgbValue >> 16) & 0xFF;
}


/**
 * Compute the green channel LED value from rgbValue config
 *
 * @return the specified value (0 - 255)
 */
int getGreenValue() {
  return (rgbValue >> 8) & 0xFF;
}


/**
 * Compute the blue channel LED value from rgbValue config
 *
 * @return the specified value (0 - 255)
 */
int getBlueValue() {
  return rgbValue & 0xFF;
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
