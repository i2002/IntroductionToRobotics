#ifndef LEADERBOARD_MANAGER_H
#define LEADERBOARD_MANAGER_H
#include <Arduino.h>
#include "config.h"


/**
 * @brief Highscore information representation
 */
struct HighscoreInfo {
  char name[leaderboardNameSize];
  byte points;
};


/**
 * @brief Manage leaderboard information.
 * 
 * Read highscores and add new highscores from / to the leaderboard store.
 */
class LeaderboardManager {
  HighscoreInfo highscoreBuf;

public:
  /**
   * @brief Checks if points would have a place in the leaderboard.
   * 
   * @param points the number of points to check
   * @return true if the points would have a place in the leaderboard, false otherwise
   */
  bool isHighscore(byte points);

  /**
   * @brief Get the highscore at a position in the leaderboard.
   * Note that this action resets the effects of setPoints() and setName() methods.
   * 
   * @param index leaderboard position
   * @return const HighscoreInfo& reference to the highscore object
   */
  const HighscoreInfo& getHighscore(int index);

  /**
   * @brief Set the current highscore number of points.
   * Note that this method resets the information provided by getHighscore().
   * 
   * @param points the current highscore points
   */
  void setPoints(byte points);

  /**
   * @brief Set the current highscore name.
   * Note that this method resets the information provided by getHighscore().
   * The contents of the name pointer are copyed to the internal buffer.
   * 
   * @param name the current highscore name
   */
  void setName(const char* name);

  /**
   * @brief Save the current highscore to the leaderboard.
   */
  void saveHighscore();

private:
  /**
   * @brief Write on EEPROM highscore information at specified position.
   * 
   * @param index leaderboard position
   * @param highscore referente to highscore information
   */
  void writeHighscore(int index, const HighscoreInfo& highscore);

  /**
   * @brief Read from the EEPROM the highscore information at specified position.
   * 
   * @param index 
   * @param highscore 
   */
  void readHighscore(int index, HighscoreInfo& highscore);
};

#endif // LEADERBOARD_MANAGER_H
