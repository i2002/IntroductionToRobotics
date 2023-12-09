#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H
#include "Input.h"

typedef void (*TextInputCallback)(const char*);

class TextInput : public Input {
  TextInputCallback preview = nullptr;
  TextInputCallback action = nullptr;
  char inputBuf[16];
  byte maxLen = 0;
  byte cursor = 0;

public:
  TextInput(const char *title, TextInputCallback _preview, TextInputCallback _action, byte _maxLen, const char* initialValue);
  ~TextInput() = default;

  void processMovement(JoystickPosition pos);

  void processActionBtn();

private:
  void setCursor(byte _cursor);

  void writeChar(char newChar);

  char nextChar();

  char prevChar();

  bool isValidChar(char c);
};

#endif // TEXT_INPUT_H
