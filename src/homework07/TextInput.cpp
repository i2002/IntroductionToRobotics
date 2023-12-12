#include "TextInput.h"
#include "context.h"

TextInput::TextInput(const char *title, byte _maxLen, const char* initialValue) :
  maxLen{_maxLen}, cursor{0}
{
  statusDisp.printTitle(title);

  for (int i = 0; i < maxLen; i++) {
    inputBuf[i] = initialValue && isValidChar(initialValue[i]) ? inputBuf[i] : ' ';
    statusDisp.printInputChar(i, inputBuf[i]);
  }

  statusDisp.printMenuArrow();
}

void TextInput::moveCursorLeft() {
  if (cursor > 0) {
    setCursor(cursor - 1);
  }
}

void TextInput::moveCursorRight() {
  if (cursor < maxLen - 1) {
    setCursor(cursor + 1);
  }
}

void TextInput::nextChar() {
  writeChar(getNextChar());
}

void TextInput::prevChar() {
  writeChar(getPrevChar());
}

const char* TextInput::getInput() const {
  return inputBuf;
}

void TextInput::setCursor(byte _cursor) {
  cursor = _cursor;
  statusDisp.printInputChar(cursor, inputBuf[cursor]);
}

void TextInput::writeChar(char newChar) {
  inputBuf[cursor] = newChar;
  statusDisp.printInputChar(cursor, inputBuf[cursor]);
}

char TextInput::getNextChar() {
  char curent = inputBuf[cursor];
  switch(curent) {
    case ' ':
      return 'a';
    case 'z':
      return 'A';
    case 'Z':
      return '0';
    case '9':
      return ' ';
    default:
      return curent + 1;
  }
}

char TextInput::getPrevChar() {
  char curent = inputBuf[cursor];
  switch(inputBuf[cursor]) {
    case ' ':
      return '9';
    case 'a':
      return ' ';
    case 'A':
      return 'z';
    case '0':
      return 'Z';
    default:
      return curent - 1;
  }
}

bool TextInput::isValidChar(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c < '0' && c > '9');
}
