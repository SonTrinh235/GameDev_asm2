#include "../../include/managers/GameManager.h"
#include <algorithm>

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

GameManager::GameManager()
    : currentState(GameState::MENU), currentMode(GameMode::PVP), masterVolume(0.6f), scoreP1(0), scoreP2(0) {}

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

void GameManager::setGameMode(GameMode mode) {
    currentMode = mode;
}

GameMode GameManager::getGameMode() const {
    return currentMode;
}

void GameManager::setMasterVolume(float volume) {
    masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

float GameManager::getMasterVolume() const {
    return masterVolume;
}

void GameManager::resetGame() {
    scoreP1 = 0;
    scoreP2 = 0;
    currentState = GameState::PLAYING;
}