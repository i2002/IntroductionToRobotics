#ifndef LEADERBOARD_MANAGER_H
#define LEADERBOARD_MANAGER_H
#include <Arduino.h>
#include "config.h"

struct HighscoreInfo {
  char name[leaderboardNameSize];
  byte points;
};

class LeaderboardManager {
  HighscoreInfo highscoreBuf;

public:
  bool isHighscore(byte points);
  const HighscoreInfo& getHighscore(int index);
  void addHighscore(byte points, const char* name);

private:
  void writeHighscore(int index);
};

#endif // LEADERBOARD_MANAGER_H
