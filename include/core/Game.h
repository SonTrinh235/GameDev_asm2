#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "Window.h"
#include "../entities/Player.h"
#include "../entities/Platform.h"
#include "../entities/Projectile.h"
#include "../entities/Item.h"
#include "../entities/WindColumn.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void handleEvents(SDL_Event* event);
    void update(float deltaTime);
    void render();
    void clean();

    bool getIsRunning() const { return isRunning; }

private:
    Window* gameWindow;
    bool isRunning;
    
    Player* player1;
    Player* player2;
    std::vector<Platform> platforms;
    std::vector<Projectile> bullets;
    std::vector<Item> items;
    std::vector<WindColumn> winds;
    float itemSpawnTimer;
    SDL_Texture* texProjectile; 
    SDL_Texture* texPlayer1;
    SDL_Texture* texPlayer2;
    SDL_Texture* texWeapon1;
    SDL_Texture* texWeapon2;
    SDL_Texture* texBackground;
    SDL_Texture* texItemHealth;
    SDL_Texture* texItemMana;
    SDL_Texture* texItemShield;
};