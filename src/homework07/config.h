#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte pinA = 3;

const byte joystickPinX = A0;
const byte joystickPinY = A1;

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

const byte triggerPin = 2;

const int leaderboardSize = 5;
const int leaderboardNameSize = 5;

const int lcdBrightnessStoreIndex = 2;
const int matrixBrightnessStoreIndex = 4;
const int soundSettingStoreIndex = 6;
const int leaderboardStoreIndex = 12;

#endif // CONFIG_H
