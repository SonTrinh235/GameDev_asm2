#pragma once

enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

class GameManager {
public:
    static GameManager& getInstance();

    void setGameState(GameState state);
    GameState getGameState() const;

    void addScoreP1(int points);
    void addScoreP2(int points);
    int getScoreP1() const;
    int getScoreP2() const;

    void resetGame();

private:
    GameManager();
    GameState currentState;
    int scoreP1;
    int scoreP2;
};