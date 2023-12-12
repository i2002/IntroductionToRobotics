#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H
#include <Arduino.h>

class TextInput {
  char inputBuf[16];
  byte maxLen = 0;
  byte cursor = 0;

public:
  TextInput(const char *title = nullptr, byte _maxLen = 0, const char* initialValue = nullptr);

  void moveCursorLeft();

  void moveCursorRight();

  void nextChar();
  
  void prevChar();

  const char* getInput() const;

private:
  void setCursor(byte _cursor);

  void writeChar(char newChar);

  char getNextChar();

  char getPrevChar();

  bool isValidChar(char c);
};

#endif // TEXT_INPUT_H
