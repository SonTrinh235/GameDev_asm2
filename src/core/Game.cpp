#include "../../include/core/Game.h"
#include "../../include/utils/Constants.h"
#include "../../include/systems/InputSystem.h"
#include "../../include/systems/AmmoSystem.h"
#include "../../include/systems/PhysicSystem.h"
#include "../../include/systems/CollisionSystem.h"
#include "../../include/systems/RenderSystem.h"
#include <iostream>
#include <algorithm>

InputSystem inputSys;
AmmoSystem ammoSys;
PhysicSystem physicsSys;
CollisionSystem collisionSys;
RenderSystem renderSys;

Game::Game() : gameWindow(nullptr), isRunning(false), 
                player1(nullptr), player2(nullptr), 
                texProjectile(nullptr), 
                texPlayer1(nullptr), texPlayer2(nullptr),
                texBackground(nullptr) {} // Khởi tạo null

Game::~Game() { clean(); }

bool Game::init(const char* title, int width, int height) {
    gameWindow = new Window();
    if (!gameWindow->init(title, width, height)) return false;

    SDL_Renderer* renderer = gameWindow->getRenderer();

    // Background
    SDL_Surface* surfBG = SDL_LoadBMP("assets/textures/background.bmp");
    if (surfBG) {
        texBackground = SDL_CreateTextureFromSurface(renderer, surfBG);
        SDL_DestroySurface(surfBG);
    }

    // Load Bullet
    SDL_Surface* surfBullet = SDL_LoadBMP("assets/textures/projectile.bmp");
    if (surfBullet) {
        SDL_SetSurfaceColorKey(surfBullet, true, SDL_MapSurfaceRGB(surfBullet, 0, 0, 0));
        texProjectile = SDL_CreateTextureFromSurface(renderer, surfBullet);
        SDL_DestroySurface(surfBullet);
    }

    // Load Players
    SDL_Surface* surfP1 = SDL_LoadBMP("assets/textures/player1.bmp");
    if (surfP1) {
        SDL_SetSurfaceColorKey(surfP1, true, SDL_MapSurfaceRGB(surfP1, 0, 0, 0));
        texPlayer1 = SDL_CreateTextureFromSurface(renderer, surfP1);
        SDL_DestroySurface(surfP1);
    }

    SDL_Surface* surfP2 = SDL_LoadBMP("assets/textures/player2.bmp");
    if (surfP2) {
        SDL_SetSurfaceColorKey(surfP2, true, SDL_MapSurfaceRGB(surfP2, 0, 0, 0));
        texPlayer2 = SDL_CreateTextureFromSurface(renderer, surfP2);
        SDL_DestroySurface(surfP2);
    }
    //Below
    platforms.push_back({70, 500, 630, 50});
    //Left 
    platforms.push_back({100, 360, 200, 20});
    //Right
    platforms.push_back({550, 345, 200, 20});
    //Center
    platforms.push_back({300, 200, 200, 20});

    player1 = new Player(1, 150, 300);
    player2 = new Player(2, 600, 300);

    isRunning = true;
    return true;
}

void Game::handleEvents(SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) isRunning = false;
}
void Game::update(float deltaTime) {
    if (!player1 || !player2) return;
    
    if (player1->hp <= 0 || player2->hp <= 0) {
        return;
    }

    int numKeys;
    const bool* keys = SDL_GetKeyboardState(&numKeys);

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
        [](const Projectile& p) { return !p.active; }), bullets.end());

    inputSys.update(*player1, deltaTime, keys);
    ammoSys.update(*player1, deltaTime, bullets);
    inputSys.update(*player2, deltaTime, keys);
    ammoSys.update(*player2, deltaTime, bullets);

    physicsSys.updatePlayer(*player1, deltaTime);
    physicsSys.updatePlayer(*player2, deltaTime);
    physicsSys.updateBullets(bullets, deltaTime); 

    collisionSys.update(*player1, bullets, platforms);
    collisionSys.update(*player2, bullets, platforms);

    if (player1->position.y > 630) {
        player1->hp = 0;
    }
    if (player2->position.y > 630) {
        player2->hp = 0;
    }

    float rotSpeed = 360.0f / ROT_TIME;
    player1->aimAngle += rotSpeed * deltaTime;
    player2->aimAngle += rotSpeed * deltaTime;
}

void Game::render() {
    if (!gameWindow) return;
    gameWindow->clear();
    SDL_Renderer* renderer = gameWindow->getRenderer();

    if (player1 && player2) {
        renderSys.render(renderer, *player1, *player2, bullets, platforms, 
                         texProjectile, texPlayer1, texPlayer2, texBackground);
    }
    
    gameWindow->display();
}

void Game::clean() {
    if (texBackground) SDL_DestroyTexture(texBackground);
    if (texProjectile) SDL_DestroyTexture(texProjectile);
    if (texPlayer1) SDL_DestroyTexture(texPlayer1);
    if (texPlayer2) SDL_DestroyTexture(texPlayer2);
    if (player1) delete player1;
    if (player2) delete player2;
    if (gameWindow) {
        gameWindow->clean();
        delete gameWindow;
    }
}