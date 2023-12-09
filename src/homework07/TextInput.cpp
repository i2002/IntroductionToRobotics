#include "TextInput.h"
#include "context.h"

TextInput::TextInput(const char *title, TextInputCallback _preview, TextInputCallback _action, byte _maxLen, const char* initialValue) :
  preview{_preview}, action{_action}, maxLen{_maxLen}, cursor{0}
{
  statusDisp.printTitle(title);

  for (int i = 0; i < maxLen; i++) {
    inputBuf[i] = initialValue && isValidChar(initialValue[i]) ? inputBuf[i] : ' ';
    statusDisp.printInputChar(i, inputBuf[i]);
  }

  statusDisp.printMenuArrow();
}

void TextInput::processMovement(JoystickPosition pos) {
  switch(pos) {
    case JoystickPosition::LEFT:
      if (cursor > 0) {
        setCursor(cursor - 1);
      }
      break;

    case JoystickPosition::RIGHT:
      if (cursor < maxLen - 1) {
        setCursor(cursor + 1);
      }
      break;

    case JoystickPosition::UP:
      writeChar(nextChar());
      break;

    case JoystickPosition::DOWN:
      writeChar(prevChar());
      break;

    case JoystickPosition::NEUTRAL:
      return;
  }

  if (preview) {
    preview(inputBuf);
  }
}

void TextInput::processActionBtn() {
  if (action) {
    action(inputBuf);
  }
}

void TextInput::setCursor(byte _cursor) {
  cursor = _cursor;
  statusDisp.printInputChar(cursor, inputBuf[cursor]);
}

void TextInput::writeChar(char newChar) {
  inputBuf[cursor] = newChar;
  statusDisp.printInputChar(cursor, inputBuf[cursor]);
}

char TextInput::nextChar() {
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

char TextInput::prevChar() {
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
