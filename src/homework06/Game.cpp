#include "Game.h"

const int Game::playerMovementMatrix[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

void Game::startGame() {
  generateMatrix();
  gameState = GameState::RUNNING;
}

bool Game::playerMove(JoystickPosition pos) {
  if (pos == JoystickPosition::NEUTRAL) {
    return true;
  }

  Position newPos = {
    playerPos.x + playerMovementMatrix[(int) pos][0],
    playerPos.y + playerMovementMatrix[(int) pos][1],
  };

  if (!validatePos(newPos)) {
    return false;
  }

  playerPos = newPos;
  return true;
}

void Game::placeBomb(unsigned long time) {
  if (bombActive) {
    return;
  }

  bombPos = playerPos;
  setCellType(bombPos, CellType::BOMB);
  bombPlacementTime = time;
  bombActive = true;
}

bool Game::bombTick(unsigned long time) {
  if (!bombActive || time - bombPlacementTime < bombExplodeTime) {
    return false;
  }

  explodeBomb();

  if (checkWinCondition()) {
    gameState = GameState::WON;
  }

  return true;
}

CellType Game::getCellType(Position pos) {
  CellType res = gameMatrix[pos.y][pos.x];
  if (res == CellType::EMPTY && pos == playerPos) {
    res = CellType::PLAYER;
  }

  return res;
}

void Game::setCellType(Position pos, CellType type) {
  gameMatrix[pos.y][pos.x] = type;
}

GameState Game::getState() {
  return gameState;
}

void Game::generateMatrix() {
  int wallsNr = random(0.5 * matrixHeight * matrixWidth, 0.75 * matrixHeight * matrixWidth + 1);
  playerPos = randomPos();

  for (int i = 0; i < wallsNr; i++) {
    Position wallPos = randomPos();

    // do not generate walls over player
    if (abs(playerPos.x - wallPos.x) < 2 && abs(playerPos.y - wallPos.y) < 2) {
      i--;
      continue;
    }

    setCellType(wallPos, CellType::WALL);
  }
}

void Game::explodeBomb() {
  bombActive = false;
  for (int row = 0; row < matrixHeight; row++) {
    Position pos = {bombPos.x, row};
    setCellType(pos, CellType::EMPTY);

    if (pos == playerPos) {
      gameState = GameState::LOST;
    }
  }

  for (int col = 0; col < matrixWidth; col++) {
    Position pos = {col, bombPos.y};
    setCellType(pos, CellType::EMPTY);

    if (pos == playerPos) {
      gameState = GameState::LOST;
    }
  }
}

bool Game::checkWinCondition() {
  for (int row = 0; row < matrixHeight; row++) {
    for (int col = 0; col < matrixWidth; col++) {
      Position pos = {col, row};
      if (pos != playerPos && getCellType(pos) != CellType::EMPTY) {
        return false;
      }
    }
  }

  return true;
}

bool Game::validatePos(Position pos) {
  if (pos.x < 0 || pos.y < 0 || pos.x >= matrixWidth || pos.y >= matrixHeight) {
    return false;
  }

  if (getCellType(pos) != CellType::EMPTY) {
    return false;
  }

  return true;
}

Position Game::randomPos() {
  return {
    random(0, matrixWidth),
    random(0, matrixHeight)
  };
}
