#include "StatusDisplay.h"
#include "context.h"

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
  analogWrite(pinA, lcdBrightnessStore.readValue());

  lcd.createChar(UP_DOWN_ARROW, upDownArrow);
  lcd.createChar(UP_ARROW, upArrow);
  lcd.createChar(DOWN_ARROW, downArrow);
  lcd.createChar(LEFT_ARROW, leftArrow);
  lcd.createChar(RIGHT_ARROW, rightArrow);
}

void StatusDisplay::printTitle(const char *name) {
  resetDisplay();
  byte start = (16 - strlen(name)) / 2;
  lcd.setCursor(start, 0);
  lcd.print(name);
}

void StatusDisplay::printMenuOption(const char *name) {
  lcd.setCursor(0, 1);
  byte pos = 0;
  pos += lcd.write('>');
  pos += lcd.write(' ');
  pos += lcd.print(name);
  printBlank(pos, 15);
}

void StatusDisplay::printRange(int step) {
  lcd.setCursor(0, 1);
  byte pos = 0;
  pos += lcd.write(LEFT_ARROW);
  for (int i = 0; i < step; i++) {
    pos += lcd.write(0b11111111);
  }

  printBlank(pos, 15);
  lcd.write(RIGHT_ARROW);
}

void StatusDisplay::printScreen(const Screen screen)  {
  resetDisplay();
  if (screen[0]) {
    lcd.print(screen[0]);
  }
  lcd.setCursor(0, 1);

  if (screen[1]) {
    lcd.print(screen[1]);
  }
}

void StatusDisplay::printLeaderboard(byte place, byte score) {
  lcd.setCursor(0, 1);
  byte pos = 0;
  pos += lcd.print(place);
  pos += lcd.write('.');
  pos += lcd.print(score);
  printBlank(pos, 15);
}

void StatusDisplay::printInputChar(byte cursor, char inputChar) {
  statusDisp.lcd.noCursor();
  statusDisp.lcd.print(inputChar);
  statusDisp.lcd.setCursor(cursor, 1);
  statusDisp.lcd.cursor();
}

void StatusDisplay::printMenuArrow(bool canPrev, bool canNext) {
  lcd.setCursor(15, 1);
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

void StatusDisplay::resetDisplay() {
  lcd.clear();
  lcd.home();
  lcd.noCursor();
}

void StatusDisplay::printBlank(byte from, byte to, byte line) {
  lcd.setCursor(from, line);
  for (byte i = from; i < to; i++) {
    lcd.write(' ');
  }
}
