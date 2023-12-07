#include "LeaderboardManager.h"
#include <EEPROM.h>
#include "config.h"
#include "context.h"


// void highscoreNameAction(char*) {
//   leaderboardManager.saveHighscore();
// }

const HighscoreInfo& LeaderboardManager::getHighscore(int index) {
  if (index < leaderboardSize) {
    EEPROM.get(leaderboardStoreIndex + index * sizeof(HighscoreInfo), highscoreBuf);
  }

  return highscoreBuf;
}

void LeaderboardManager::writeHighscore(int index) {
  if (index < leaderboardSize) {
    EEPROM.put(leaderboardStoreIndex + index * sizeof(HighscoreInfo), highscoreBuf);
  }
}

bool LeaderboardManager::isHighscore(byte points) {
  for (int i = 0; i < 5; i++) {
    if(points >= getHighscore(i).points) {
      return true;
    }
  }

  return false;
}

// void LeaderboardManager::setLeaderboardName() {
//   inputManager.setupTextInput("Highscore name", nullptr, highscoreNameAction, highscoreBuf.name, 5);
//   // return highscoreBuf.name;
// }

void LeaderboardManager::addHighscore(byte points, const char* name) {
  int indexInsert = leaderboardSize - 1;
  for (int i = indexInsert - 1; i >= 0; i--) {
    if(points >= getHighscore(i).points) {
      writeHighscore(i + 1);
      indexInsert = i;
    } else {
      break;
    }
  }

  highscoreBuf.points = points;
  memcpy(highscoreBuf.name, name, leaderboardNameSize);
  writeHighscore(indexInsert);
}
