#include "LeaderboardManager.h"
#include <EEPROM.h>
#include "config.h"
#include "context.h"


bool LeaderboardManager::isHighscore(byte points) {
  HighscoreInfo highscore;
  for (int i = 0; i < leaderboardSize; i++) {
    readHighscore(i, highscore);
    if(points >= highscore.points || highscore.points == maxHighscorePoints) {
      return true;
    }
  }

  return false;
}

const HighscoreInfo& LeaderboardManager::getHighscore(int index) {
  readHighscore(index, highscoreBuf);
  return highscoreBuf;
}

void LeaderboardManager::setPoints(byte points) {
  highscoreBuf.points = points;
}

void LeaderboardManager::setName(const char* name) {
  memcpy(highscoreBuf.name, name, leaderboardNameSize);
}

void LeaderboardManager::saveHighscore() {
  int indexInsert = leaderboardSize - 1;
  HighscoreInfo tempHighscore;
  for (int i = indexInsert - 1; i >= 0; i--) {
    readHighscore(i, tempHighscore);
    if(highscoreBuf.points >= tempHighscore.points || tempHighscore.points == maxHighscorePoints) {
      writeHighscore(i + 1, tempHighscore);
      indexInsert = i;
    } else {
      break;
    }
  }
  
  writeHighscore(indexInsert, highscoreBuf);
}

void LeaderboardManager::writeHighscore(int index, const HighscoreInfo& highscore) {
  if (index < leaderboardSize) {
    EEPROM.put(leaderboardStoreIndex + index * sizeof(HighscoreInfo), highscore);
  }
}

void LeaderboardManager::readHighscore(int index, HighscoreInfo& highscore) {
  if (index < leaderboardSize) {
    EEPROM.get(leaderboardStoreIndex + index * sizeof(HighscoreInfo), highscore);
  }
}
