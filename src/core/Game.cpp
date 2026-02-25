#include "../../include/core/Game.h"
#include "../../include/utils/Constants.h"
#include "../../include/systems/InputSystem.h"
#include "../../include/systems/AmmoSystem.h"
#include "../../include/systems/PhysicsSystem.h"
#include "../../include/systems/CollisionSystem.h"
#include "../../include/systems/RenderSystem.h"
#include "../../include/managers/ResourceManager.h"
#include "../../include/managers/GameManager.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime> 
#include <SDL3/SDL.h>

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
                texBlast(nullptr), // Khởi tạo texture mới
                itemSpawnTimer(0.0f),
                bgmStream(nullptr), 
                bgmAudioData(nullptr), 
                bgmAudioLen(0) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int width, int height) {
    gameWindow = new Window();
    if (!gameWindow->init(title, width, height)) return false;

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec wavSpec;
    if (!SDL_LoadWAV("assets/sounds/08 - Lake.wav", &wavSpec, &bgmAudioData, &bgmAudioLen)) {
        std::cout << "Khong the tai nhac nen: " << SDL_GetError() << std::endl;
    } else {
        bgmStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wavSpec, nullptr, nullptr);
        if (bgmStream) {
            SDL_PutAudioStreamData(bgmStream, bgmAudioData, bgmAudioLen);
            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(bgmStream));
        } else {
            std::cout << "Loi tao Audio Stream: " << SDL_GetError() << std::endl;
        }
    }
    SDL_Renderer* renderer = gameWindow->getRenderer();

    ResourceManager::getInstance().loadTexture(renderer, "bg", "assets/textures/background1.bmp");
    texBackground = ResourceManager::getInstance().getTexture("bg");

    ResourceManager::getInstance().loadTexture(renderer, "bullet", "assets/textures/projectile.bmp");
    texProjectile = ResourceManager::getInstance().getTexture("bullet");

    ResourceManager::getInstance().loadTexture(renderer, "p1", "assets/textures/player1.bmp");
    texPlayer1 = ResourceManager::getInstance().getTexture("p1");

    ResourceManager::getInstance().loadTexture(renderer, "p2", "assets/textures/player2.bmp");
    texPlayer2 = ResourceManager::getInstance().getTexture("p2");

    ResourceManager::getInstance().loadTexture(renderer, "w1", "assets/textures/weapon1.bmp");
    texWeapon1 = ResourceManager::getInstance().getTexture("w1");

    ResourceManager::getInstance().loadTexture(renderer, "w2", "assets/textures/weapon2.bmp");
    texWeapon2 = ResourceManager::getInstance().getTexture("w2");

    ResourceManager::getInstance().loadTexture(renderer, "health", "assets/textures/health.bmp");
    texItemHealth = ResourceManager::getInstance().getTexture("health");

    ResourceManager::getInstance().loadTexture(renderer, "mana", "assets/textures/power.bmp");
    texItemMana = ResourceManager::getInstance().getTexture("mana");

    ResourceManager::getInstance().loadTexture(renderer, "shield", "assets/textures/shield.bmp");
    texItemShield = ResourceManager::getInstance().getTexture("shield");

    // Load Blast.bmp cho hiệu ứng nổ
    ResourceManager::getInstance().loadTexture(renderer, "blast", "assets/textures/Blast.bmp");
    texBlast = ResourceManager::getInstance().getTexture("blast");

    // Player 1 textures
    ResourceManager::getInstance().loadTexture(renderer, "Dark_1", "assets/textures/Dark_1.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Dark_2", "assets/textures/Dark_2.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Dark_3", "assets/textures/Dark_3.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Ultimate_P1", "assets/textures/Brimstone_Gigablast.bmp");
    
    // Player 2 textures
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_1", "assets/textures/Nebula_1.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_2", "assets/textures/Nebula_2.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_3", "assets/textures/Nebula_3.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Ultimate_P2", "assets/textures/Death_Fire.bmp");

    // Platforms
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

    platforms.push_back({200, 430, 350, 20}); 
    platforms.push_back({900, 410, 280, 20}); 

    float windWidth = 160.0f;
    float windHeight = 600.0f;
    float windX = 630.0f;
    float windY = 600.0f - windHeight;
    
    winds.push_back(WindColumn(windX, windY, windWidth, windHeight, GRAVITY * 1.5f, 400.0f));

    player1 = new Player(1, 332, 100); 
    player2 = new Player(2, 1016, 100);

    GameManager::getInstance().setGameState(GameState::PLAYING);

    srand((unsigned int)time(NULL));
    isRunning = true;
    return true;
}

void Game::handleEvents(SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) isRunning = false;
}

void Game::update(float deltaTime) {
    if (!player1 || !player2) return;
    
    if (bgmStream && bgmAudioData) {
        if (SDL_GetAudioStreamAvailable(bgmStream) < (int)(bgmAudioLen / 2)) {
            SDL_PutAudioStreamData(bgmStream, bgmAudioData, bgmAudioLen);
        }
    }

    int numKeys;
    const bool* keys = SDL_GetKeyboardState(&numKeys);

    if (GameManager::getInstance().getGameState() == GameState::GAME_OVER) {
        if (keys[SDL_SCANCODE_R]) {
            player1->hp = player1->maxHp;
            player2->hp = player2->maxHp;
            player1->mana = player1->maxMana;
            player2->mana = player2->maxMana;
            
            player1->position.x = 332; player1->position.y = 100;
            player2->position.x = 1016; player2->position.y = 100;
            player1->velocity.x = 0; player1->velocity.y = 0;
            player2->velocity.x = 0; player2->velocity.y = 0;
            
            bullets.clear();
            items.clear();
            explosions.clear(); // Xóa sạch vụ nổ khi reset
            itemSpawnTimer = 0.0f;
            
            GameManager::getInstance().resetGame();
        }
        return; 
    }

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
    physicsSys.updateExplosions(explosions, deltaTime); // Cập nhật animation vụ nổ

    // Cập nhật va chạm cho cả 2 player và truyền vector explosions
    collisionSys.update(*player1, bullets, platforms, items, explosions);
    collisionSys.update(*player2, bullets, platforms, items, explosions);

    if (player1->position.y > 630) player1->hp = 0;
    if (player2->position.y > 630) player2->hp = 0;

    if (player1->hp <= 0 || player2->hp <= 0) {
        GameManager::getInstance().setGameState(GameState::GAME_OVER);
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
        // Truyền thêm explosions và texBlast vào RenderSystem
        renderSys.render(renderer, *player1, *player2, bullets, platforms, items, winds, explosions,
                         texProjectile, texPlayer1, texPlayer2, texBackground, 
                         texWeapon1, texWeapon2,
                         texItemHealth, texItemMana, texItemShield, texBlast,
                         ResourceManager::getInstance());

        if (GameManager::getInstance().getGameState() == GameState::GAME_OVER) {
             SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
             SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
             SDL_RenderFillRect(renderer, &overlay);
        }
    }
    
    gameWindow->display();
}

void Game::clean() {
    if (bgmStream) {
        SDL_DestroyAudioStream(bgmStream);
        bgmStream = nullptr;
    }
    if (bgmAudioData) {
        SDL_free(bgmAudioData);
        bgmAudioData = nullptr;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    ResourceManager::getInstance().clean();
    if (player1) delete player1;
    if (player2) delete player2;
    if (gameWindow) {
        gameWindow->clean();
        delete gameWindow;
    }
}