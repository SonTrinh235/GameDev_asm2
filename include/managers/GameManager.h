#pragma once

enum class GameState {
    MENU,
    SETTINGS,
    HOW_TO_PLAY,
    PLAYING,
    PAUSED,
    GAME_OVER
};

enum class GameMode {
    PVP,
    PVE
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

    void setGameMode(GameMode mode);
    GameMode getGameMode() const;

    void setMasterVolume(float volume);
    float getMasterVolume() const;

    void resetGame();

private:
    GameManager();
    GameState currentState;
    GameMode currentMode;
    float masterVolume;
    int scoreP1;
    int scoreP2;
};