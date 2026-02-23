#include "../../include/managers/GameManager.h"

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

GameManager::GameManager() : currentState(GameState::PLAYING), scoreP1(0), scoreP2(0) {}

void GameManager::setGameState(GameState state) {
    currentState = state;
}

GameState GameManager::getGameState() const {
    return currentState;
}

void GameManager::addScoreP1(int points) { scoreP1 += points; }
void GameManager::addScoreP2(int points) { scoreP2 += points; }

int GameManager::getScoreP1() const { return scoreP1; }
int GameManager::getScoreP2() const { return scoreP2; }

void GameManager::resetGame() {
    scoreP1 = 0;
    scoreP2 = 0;
    currentState = GameState::PLAYING;
}