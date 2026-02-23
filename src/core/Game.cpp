#include "../../include/core/Game.h"
#include "../../include/utils/Constants.h"
#include "../../include/systems/InputSystem.h"
#include "../../include/systems/AmmoSystem.h"
#include "../../include/systems/PhysicsSystem.h"
#include "../../include/systems/CollisionSystem.h"
#include "../../include/systems/RenderSystem.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime> 

InputSystem inputSys;
AmmoSystem ammoSys;
PhysicSystem physicsSys;
CollisionSystem collisionSys;
RenderSystem renderSys;

Game::Game() : gameWindow(nullptr), isRunning(false), 
               player1(nullptr), player2(nullptr), 
               texProjectile(nullptr), 
               texPlayer1(nullptr), texPlayer2(nullptr),
               texWeapon1(nullptr), texWeapon2(nullptr),
               texBackground(nullptr), 
               texItemHealth(nullptr), texItemMana(nullptr), texItemShield(nullptr),
               itemSpawnTimer(0.0f) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int width, int height) {
    gameWindow = new Window();
    if (!gameWindow->init(title, width, height)) return false;

    SDL_Renderer* renderer = gameWindow->getRenderer();

    // Background
    SDL_Surface* surfBG = SDL_LoadBMP("assets/textures/background1.bmp");
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

    // Load Weapons
    SDL_Surface* surfW1 = SDL_LoadBMP("assets/textures/weapon1.bmp");
    if (surfW1) {
        SDL_SetSurfaceColorKey(surfW1, true, SDL_MapSurfaceRGB(surfW1, 0, 0, 0));
        texWeapon1 = SDL_CreateTextureFromSurface(renderer, surfW1);
        SDL_DestroySurface(surfW1);
    }

    SDL_Surface* surfW2 = SDL_LoadBMP("assets/textures/weapon2.bmp");
    if (surfW2) {
        SDL_SetSurfaceColorKey(surfW2, true, SDL_MapSurfaceRGB(surfW2, 0, 0, 0));
        texWeapon2 = SDL_CreateTextureFromSurface(renderer, surfW2);
        SDL_DestroySurface(surfW2);
    }

    SDL_Surface* surfHeal = SDL_LoadBMP("assets/textures/health.bmp");
    if (surfHeal) {
        SDL_SetSurfaceColorKey(surfHeal, true, SDL_MapSurfaceRGB(surfHeal, 0, 0, 0));
        texItemHealth = SDL_CreateTextureFromSurface(renderer, surfHeal);
        SDL_DestroySurface(surfHeal);
    }

    SDL_Surface* surfMana = SDL_LoadBMP("assets/textures/power.bmp");
    if (surfMana) {
        SDL_SetSurfaceColorKey(surfMana, true, SDL_MapSurfaceRGB(surfMana, 0, 0, 0));
        texItemMana = SDL_CreateTextureFromSurface(renderer, surfMana);
        SDL_DestroySurface(surfMana);
    }

    SDL_Surface* surfShield = SDL_LoadBMP("assets/textures/shield.bmp");
    if (surfShield) {
        SDL_SetSurfaceColorKey(surfShield, true, SDL_MapSurfaceRGB(surfShield, 0, 0, 0));
        texItemShield = SDL_CreateTextureFromSurface(renderer, surfShield);
        SDL_DestroySurface(surfShield);
    }

    // Platforms
    // Below 1
    platforms.push_back({186, 600, 54, 15});
    platforms.push_back({230, 596, 54, 15});
    platforms.push_back({274, 591, 54, 15});
    platforms.push_back({318, 587, 54, 15});
    platforms.push_back({362, 582, 54, 15});
    platforms.push_back({406, 578, 54, 15});
    platforms.push_back({450, 573, 54, 15});
    platforms.push_back({494, 569, 54, 15});
    platforms.push_back({538, 564, 54, 15});
    platforms.push_back({582, 560, 44, 15});

    //Below 2
    platforms.push_back({790, 560, 54, 15});
    platforms.push_back({834, 564, 54, 15});
    platforms.push_back({878, 569, 54, 15});
    platforms.push_back({922, 573, 54, 15});
    platforms.push_back({966, 578, 54, 15});
    platforms.push_back({1010, 582, 54, 15});
    platforms.push_back({1054, 587, 54, 15});
    platforms.push_back({1098, 591, 54, 15});
    platforms.push_back({1142, 596, 54, 15});
    platforms.push_back({1186, 600, 44, 15});

    // Middle
    platforms.push_back({200, 430, 350, 20});  // Left 
    platforms.push_back({900, 410, 280, 20});  // Right
    // platforms.push_back({300, 200, 200, 20});  // Center


    float windWidth = 160.0f;
    float windHeight = 600.0f;
    float windX = 630.0f;
    float windY = 600.0f - windHeight;
    
    winds.push_back(WindColumn(windX, windY, windWidth, windHeight, GRAVITY * 1.5f, 400.0f));

    player1 = new Player(1, 332, 100); 
    player2 = new Player(2, 1016, 100);

    srand((unsigned int)time(NULL));
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

    if (player1->shieldTimer > 0) player1->shieldTimer -= deltaTime;
    if (player1->infiniteManaTimer > 0) {
        player1->infiniteManaTimer -= deltaTime;
        player1->mana = player1->maxMana;
    }

    if (player2->shieldTimer > 0) player2->shieldTimer -= deltaTime;
    if (player2->infiniteManaTimer > 0) {
        player2->infiniteManaTimer -= deltaTime;
        player2->mana = player2->maxMana;
    }

    itemSpawnTimer += deltaTime;
    if (itemSpawnTimer >= 8.0f && !platforms.empty()) { 
        itemSpawnTimer = 0.0f;
        
        int platIndex = rand() % platforms.size(); 
        SDL_FRect platRect = platforms[platIndex].getRect();
        
        int minX = (int)platRect.x + 20;
        int maxX = (int)(platRect.x + platRect.w - 40);
        
        if (maxX <= minX) maxX = minX + 1; 

        float spawnX = minX + (rand() % (maxX - minX));
        float spawnY = platRect.y - 30.0f; 

        ItemType type = (ItemType)(rand() % 3);
        items.push_back(Item(spawnX, spawnY, type));
    }

    items.erase(std::remove_if(items.begin(), items.end(), 
        [](const Item& i) { return !i.active; }), items.end());

    inputSys.update(*player1, deltaTime, keys);
    ammoSys.update(*player1, deltaTime, bullets);
    inputSys.update(*player2, deltaTime, keys);
    ammoSys.update(*player2, deltaTime, bullets);

    physicsSys.updatePlayer(*player1, winds, deltaTime);
    physicsSys.updatePlayer(*player2, winds, deltaTime);
    physicsSys.updateBullets(bullets, winds, deltaTime); 

    collisionSys.update(*player1, bullets, platforms, items);
    collisionSys.update(*player2, bullets, platforms, items);

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
        renderSys.render(renderer, *player1, *player2, bullets, platforms, items, winds,
                         texProjectile, texPlayer1, texPlayer2, texBackground, 
                         texWeapon1, texWeapon2,
                         texItemHealth, texItemMana, texItemShield);

        if (player1->hp <= 0 || player2->hp <= 0) {
             SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
             SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
             SDL_RenderFillRect(renderer, &overlay);
        }
    }
    
    gameWindow->display();
}

void Game::clean() {
    if (texBackground) SDL_DestroyTexture(texBackground);
    if (texWeapon1) SDL_DestroyTexture(texWeapon1);
    if (texWeapon2) SDL_DestroyTexture(texWeapon2);
    if (texProjectile) SDL_DestroyTexture(texProjectile);
    if (texPlayer1) SDL_DestroyTexture(texPlayer1);
    if (texPlayer2) SDL_DestroyTexture(texPlayer2);
    
    if (texItemHealth) SDL_DestroyTexture(texItemHealth);
    if (texItemMana) SDL_DestroyTexture(texItemMana);
    if (texItemShield) SDL_DestroyTexture(texItemShield);

    if (player1) delete player1;
    if (player2) delete player2;
    if (gameWindow) {
        gameWindow->clean();
        delete gameWindow;
    }
}