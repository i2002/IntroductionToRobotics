#include "TextInput.h"
#include "context.h"

TextInput::TextInput(const char *title, TextInputCallback _preview, TextInputCallback _action, char* _inputBuf, byte _maxLen) :
  preview{_preview}, action{_action}, inputBuf{_inputBuf}, maxLen{_maxLen}, cursor{0}
{
  statusDisp.printTitle(title);

  for (int i = 0; i < maxLen; i++) {
    if ((inputBuf[i] < 'A' || inputBuf[i] > 'Z') && (inputBuf[i] < 'a' || inputBuf[i] > 'z') && (inputBuf[i] < '0' || inputBuf[i] > '9')) {
      inputBuf[i] = ' ';
    }
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
  switch(inputBuf[cursor]) {
    case ' ':
      return 'a';
    case 'z':
      return 'A';
    case 'Z':
      return '0';
    case '9':
      return ' ';
    default:
      return inputBuf[cursor] + 1;
  }
}

char TextInput::prevChar() {
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
      return inputBuf[cursor] - 1;
  }
}
