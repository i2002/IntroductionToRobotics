#include "StatusDisplay.h"

// custom chars
uint8_t upDownArrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte upArrow[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte downArrow[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte leftArrow[8] = {
	0b00010,
	0b00110,
	0b01110,
	0b11110,
	0b01110,
	0b00110,
	0b00010,
	0b00000
};

byte rightArrow[8] = {
  0b01000,
	0b01100,
	0b01110,
	0b01111,
	0b01110,
	0b01100,
	0b01000,
	0b00000
};


void StatusDisplay::setup() {
  lcd.begin(16, 2);
  analogWrite(pinA, 100);

  lcd.createChar(UP_DOWN_ARROW, upDownArrow);
  lcd.createChar(UP_ARROW, upArrow);
  lcd.createChar(DOWN_ARROW, downArrow);
  lcd.createChar(LEFT_ARROW, leftArrow);
  lcd.createChar(RIGHT_ARROW, rightArrow);
}

void StatusDisplay::setupMenu(const char *name) {
  resetDisplay();
  int start = (16 - strlen(name)) / 2;
  lcd.setCursor(start, 0);
  lcd.print(name);
}

void StatusDisplay::printMenuOption(const char *name, bool canPrev, bool canNext) {
  lcd.setCursor(0, 1);

  lcd.write('>');
  lcd.write(' ');
  lcd.print(name);

  for (int i = strlen(name); i < 13; i++) {
    lcd.write(' ');
  }

  printMenuArrow(canPrev, canNext);
}

void StatusDisplay::printMenuArrow(bool canPrev, bool canNext) {
  if (canPrev && canNext) {
    lcd.write(UP_DOWN_ARROW);
  } else if (canPrev) {
    lcd.write(UP_ARROW);
  } else if (canNext) {
    lcd.write(DOWN_ARROW);
  } else {
    lcd.write(' ');
  }
}

void StatusDisplay::resetDisplay() {
  lcd.clear();
  lcd.home();
}

void StatusDisplay::setupGameInfo(int level) {
  resetDisplay();

  lcd.print("Level ");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("@");

  updatePoints(0);
}

void StatusDisplay::updatePoints(int points) {
  lcd.setCursor(1, 1);
  lcd.print(points);
}
