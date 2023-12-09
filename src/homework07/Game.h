#ifndef GAME_H
#define GAME_H

#include "Joystick.h"

/**
 * @brief The current state of the game.
 */
enum class GameState {
  NOT_STARTED, RUNNING, LOST, WON
};


/**
 * @brief Represents the type of game matrix cell.
 */
enum class CellType {
  EMPTY, WALL, BOMB, PLAYER
};


/**
 * @brief Represents a position on the game matrix
 */
struct Position {
  int x;
  int y;

  bool operator==(const Position &other) {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Position &other) {
    return x != other.x || y != other.y;
  }
};


/**
 * @brief Manage the game state and actions.
 */
class Game {
  // Game configuration
  static const int matrixHeight = 16;
  static const int matrixWidth = 16;
  static const int maxBombs = 5;
  static const unsigned long bombExplodeTime = 2000;

  // Player movement matrix
  static const int playerMovementMatrix[4][2];

  // Game state
  GameState gameState = GameState::NOT_STARTED;
  CellType gameMatrix[matrixHeight][matrixWidth];
  Position playerPos;
  Position bombPos;
  bool bombActive = false;
  unsigned long bombPlacementTime;
  int points = 0;

public:
  /**
   * @brief Initialize game matrix and player position.
   * The walls and player position are randomly generated.
   */
  void startGame();

  /**
   * @brief Move the current player position on the game map.
   * 
   * @param pos the joystick position which indicates the direction
   * @return true the player position changed
   * @return false the player position could not be changed (wall intersection)
   */
  bool playerMove(JoystickPosition pos);

  /**
   * @brief Place the bomb on current player position.
   * If another bomb is already placed but has not exploded yet, this action is ignored.
   * 
   * @param time the current time of bomb placement
   */
  void placeBomb(unsigned long time);

  /**
   * @brief Check the bomb timeout if a bomb is active and explode if bomb interval exceeded.
   * The game state can change to LOST (if player killed by bomb) or WON (if all walls have been exploded)
   * 
   * @param time the current time
   * @return true if the bomb has been exploded
   * @return false if the bomb didn't explode (either no bomb placed, or the timeout hasn't been reached)
   */
  bool bombTick(unsigned long time);

  /**
   * @brief Get the type of a game matrix cell relative to the view offset.
   * The display matrix is smaller than the game matrix and only a fraction of the game matrix is shown.
   * The viewport is determined based on current player position.
   * 
   * @param pos 
   * @return CellType 
   */
  CellType getViewportCellType(Position pos) const;

  /**
   * @brief Get the game state.
   * 
   * @return GameState
   */
  GameState getState();

  /**
   * @brief Get the current game points
   * 
   * @return int 
   */
  int getPoints();

private:
  /**
   * @brief Set the tyep of a game matrix cell.
   * 
   * @param pos the position in game matrix
   * @param type the new type of the cell
   */
  void setCellType(Position pos, CellType type);

  /**
   * @brief Get the type of a game matrix cell.
   * 
   * @param pos the position in game matrix
   * @return CellType 
   */
  CellType getCellType(Position pos) const;

  /**
   * @brief Randomly initialize walls in game matrix and initial player position.
   */
  void generateMatrix();

  /**
   * @brief Explode the row and column of the bomb.
   */
  void explodeBomb();

  /**
   * @brief Checks if the current game matrix state is a winning condition.
   * 
   * @return true if the player won the game
   * @return false if the player hasn't yet won the game
   */
  bool checkWinCondition();

  /**
   * @brief Validate new player position.
   * 
   * @param pos the new player position
   * @return true if the new position is valid
   * @return false if the new position is invalid (outside the matrix bounds or wall collision)
   */
  bool validatePos(Position pos);

  /**
   * @brief Generate a valid random position inside game matrix.
   * 
   * @return Position 
   */
  Position randomPos();
};

#endif // GAME_H
