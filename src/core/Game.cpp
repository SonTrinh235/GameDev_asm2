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
#include <cmath>
#include <SDL3/SDL.h>

InputSystem inputSys;
AmmoSystem ammoSys;
PhysicSystem physicsSys;
CollisionSystem collisionSys;
RenderSystem renderSys;

namespace {
constexpr float MENU_BUTTON_WIDTH = 320.0f;
constexpr float MENU_BUTTON_HEIGHT = 50.0f;
constexpr float MENU_BUTTON_GAP = 16.0f;
constexpr float MENU_BUTTON_START_Y = 220.0f;

constexpr float SETTINGS_SLIDER_X = (SCREEN_WIDTH - 360.0f) * 0.5f;
constexpr float SETTINGS_SLIDER_Y = 286.0f;
constexpr float SETTINGS_SLIDER_W = 360.0f;
constexpr float SETTINGS_SLIDER_H = 18.0f;

float randomRange(float minValue, float maxValue) {
    float t = (float)rand() / (float)RAND_MAX;
    return minValue + t * (maxValue - minValue);
}

float shortestAngleDelta(float fromDeg, float toDeg) {
    float delta = toDeg - fromDeg;
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;
    return delta;
}
}

Game::Game() : gameWindow(nullptr), isRunning(false), 
                player1(nullptr), player2(nullptr), 
                texProjectile(nullptr), 
                texPlayer1(nullptr), texPlayer2(nullptr),
                texWeapon1(nullptr), texWeapon2(nullptr),
                texBackground(nullptr), 
                texItemHealth(nullptr), texItemMana(nullptr), texItemShield(nullptr),
                texBlast(nullptr), 
                itemSpawnTimer(0.0f),
                gameOverTimer(0.0f), 
                bgmStream(nullptr), 
                bgmAudioData(nullptr), 
                bgmAudioLen(0),
                battleBgmStream(nullptr),    
                battleBgmAudioData(nullptr), 
                battleBgmAudioLen(0),        
                isDraggingVolumeSlider(false),
                aiMoveSwitchTimer(0.0f),
                aiJumpCooldownTimer(0.0f),
                aiShootDecisionTimer(0.0f),
                aiTargetChargeTime(0.35f),
                aiUltimateDecisionTimer(0.0f),
                aiMoveDirection(-1) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int width, int height) {
    gameWindow = new Window();
    if (!gameWindow->init(title, width, height)) return false;

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    
    SDL_AudioSpec wavSpec;
    if (!SDL_LoadWAV("assets/sounds/08 - Lake.wav", &wavSpec, &bgmAudioData, &bgmAudioLen)) {
        std::cout << "Khong the tai nhac nen Menu: " << SDL_GetError() << std::endl;
    } else {
        bgmStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wavSpec, nullptr, nullptr);
        if (bgmStream) {
            SDL_SetAudioStreamGain(bgmStream, GameManager::getInstance().getMasterVolume());
            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(bgmStream));
        }
    }

    SDL_AudioSpec battleSpec;
    if (!SDL_LoadWAV("assets/sounds/58 - Battle! (Cynthia).wav", &battleSpec, &battleBgmAudioData, &battleBgmAudioLen)) {
        std::cout << "Khong the tai nhac Battle: " << SDL_GetError() << std::endl;
    } else {
        battleBgmStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &battleSpec, nullptr, nullptr);
        if (battleBgmStream) {
            SDL_SetAudioStreamGain(battleBgmStream, GameManager::getInstance().getMasterVolume());
            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(battleBgmStream));
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

    ResourceManager::getInstance().loadTexture(renderer, "blast", "assets/textures/Blast.bmp");
    texBlast = ResourceManager::getInstance().getTexture("blast");

    ResourceManager::getInstance().loadTexture(renderer, "Dark_1", "assets/textures/Dark_1.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Dark_2", "assets/textures/Dark_2.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Dark_3", "assets/textures/Dark_3.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Ultimate_P1", "assets/textures/Brimstone_Gigablast.bmp");
    
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_1", "assets/textures/Nebula_1.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_2", "assets/textures/Nebula_2.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Nebula_3", "assets/textures/Nebula_3.bmp");
    ResourceManager::getInstance().loadTexture(renderer, "Ultimate_P2", "assets/textures/Death_Fire.bmp");

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

    GameManager::getInstance().setGameState(GameState::MENU);

    srand((unsigned int)time(NULL));
    isRunning = true;
    return true;
}

void Game::handleEvents(SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) isRunning = false;

    if (!gameWindow) return;
    SDL_Renderer* renderer = gameWindow->getRenderer();
    GameManager& manager = GameManager::getInstance();
    GameState currentState = manager.getGameState();

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            if (currentState == GameState::PLAYING) {
                manager.setGameState(GameState::PAUSED); 
            } else if (currentState == GameState::PAUSED) {
                manager.setGameState(GameState::PLAYING); 
            } else if (currentState == GameState::SETTINGS || currentState == GameState::HOW_TO_PLAY) {
                manager.setGameState(GameState::MENU);
            }
        } else if (currentState == GameState::GAME_OVER && event->key.scancode == SDL_SCANCODE_M) {
            manager.setGameState(GameState::MENU);
        }
    }

    if (!renderer) return;
    SDL_Event converted = *event;
    SDL_ConvertEventToRenderCoordinates(renderer, &converted);

    if (converted.type == SDL_EVENT_MOUSE_BUTTON_DOWN && converted.button.button == SDL_BUTTON_LEFT) {
        float mouseX = converted.button.x;
        float mouseY = converted.button.y;

        if (currentState == GameState::MENU) {
            float left = (SCREEN_WIDTH - MENU_BUTTON_WIDTH) * 0.5f;
            SDL_FRect pvpButton = { left, MENU_BUTTON_START_Y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT };
            SDL_FRect pveButton = { left, MENU_BUTTON_START_Y + (MENU_BUTTON_HEIGHT + MENU_BUTTON_GAP), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT };
            SDL_FRect settingButton = { left, MENU_BUTTON_START_Y + 2.0f * (MENU_BUTTON_HEIGHT + MENU_BUTTON_GAP), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT };
            SDL_FRect howToPlayButton = { left, MENU_BUTTON_START_Y + 3.0f * (MENU_BUTTON_HEIGHT + MENU_BUTTON_GAP), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT };
            SDL_FRect quitButton = { left, MENU_BUTTON_START_Y + 4.0f * (MENU_BUTTON_HEIGHT + MENU_BUTTON_GAP), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT };

            if (pointInRect(mouseX, mouseY, pvpButton)) {
                startMatch(GameMode::PVP);
            } else if (pointInRect(mouseX, mouseY, pveButton)) {
                startMatch(GameMode::PVE);
            } else if (pointInRect(mouseX, mouseY, settingButton)) {
                manager.setGameState(GameState::SETTINGS);
            } else if (pointInRect(mouseX, mouseY, howToPlayButton)) {
                manager.setGameState(GameState::HOW_TO_PLAY);
            } else if (pointInRect(mouseX, mouseY, quitButton)) {
                isRunning = false;
            }
        } else if (currentState == GameState::PLAYING) {
            float btnWidth = 40.0f;
            float pauseX = (SCREEN_WIDTH - btnWidth) * 0.5f;
            SDL_FRect pauseBtn = { pauseX, 20.0f, btnWidth, 40.0f };
            if (pointInRect(mouseX, mouseY, pauseBtn)) {
                manager.setGameState(GameState::PAUSED);
            }
        } else if (currentState == GameState::PAUSED) {
            float btnWidth = 240.0f;
            float left = (SCREEN_WIDTH - btnWidth) * 0.5f;
            SDL_FRect resumeBtn = { left, SCREEN_HEIGHT * 0.45f, btnWidth, 50.0f };
            SDL_FRect menuBtn = { left, SCREEN_HEIGHT * 0.45f + 70.0f, btnWidth, 50.0f };

            if (pointInRect(mouseX, mouseY, resumeBtn)) {
                manager.setGameState(GameState::PLAYING);
            } else if (pointInRect(mouseX, mouseY, menuBtn)) {
                manager.setGameState(GameState::MENU);
            }
        } else if (currentState == GameState::SETTINGS) {
            SDL_FRect sliderRect = { SETTINGS_SLIDER_X, SETTINGS_SLIDER_Y, SETTINGS_SLIDER_W, SETTINGS_SLIDER_H };
            SDL_FRect backButton = { (SCREEN_WIDTH - 220.0f) * 0.5f, 420.0f, 220.0f, 52.0f };

            if (pointInRect(mouseX, mouseY, sliderRect)) {
                isDraggingVolumeSlider = true;
                setMasterVolumeFromX(mouseX);
            } else if (pointInRect(mouseX, mouseY, backButton)) {
                manager.setGameState(GameState::MENU);
            }
        } else if (currentState == GameState::HOW_TO_PLAY) {
            SDL_FRect backButton = { (SCREEN_WIDTH - 220.0f) * 0.5f, 570.0f, 220.0f, 52.0f };
            if (pointInRect(mouseX, mouseY, backButton)) {
                manager.setGameState(GameState::MENU);
            }
        }
    } else if (converted.type == SDL_EVENT_MOUSE_MOTION && isDraggingVolumeSlider && currentState == GameState::SETTINGS) {
        setMasterVolumeFromX(converted.motion.x);
    } else if (converted.type == SDL_EVENT_MOUSE_BUTTON_UP && converted.button.button == SDL_BUTTON_LEFT) {
        isDraggingVolumeSlider = false;
    }
}

void Game::update(float deltaTime) {
    if (!player1 || !player2) return;
    
    GameManager& manager = GameManager::getInstance();
    GameState gameState = manager.getGameState();

    static GameState lastState = GameState::MENU;
    bool wasBattle = (lastState == GameState::PLAYING || lastState == GameState::PAUSED || lastState == GameState::GAME_OVER);
    bool isBattle = (gameState == GameState::PLAYING || gameState == GameState::PAUSED || gameState == GameState::GAME_OVER);

    if (wasBattle != isBattle) {
        if (isBattle) {
            if (bgmStream) SDL_ClearAudioStream(bgmStream); 
        } else {
            if (battleBgmStream) SDL_ClearAudioStream(battleBgmStream); 
        }
    }
    lastState = gameState;

    if (isBattle) {
        if (battleBgmStream && battleBgmAudioData) {
            if (SDL_GetAudioStreamAvailable(battleBgmStream) < (int)(battleBgmAudioLen / 2)) {
                SDL_PutAudioStreamData(battleBgmStream, battleBgmAudioData, battleBgmAudioLen);
            }
        }
    } else {
        if (bgmStream && bgmAudioData) {
            if (SDL_GetAudioStreamAvailable(bgmStream) < (int)(bgmAudioLen / 2)) {
                SDL_PutAudioStreamData(bgmStream, bgmAudioData, bgmAudioLen);
            }
        }
    }

    int numKeys;
    const bool* keys = SDL_GetKeyboardState(&numKeys);

    if (gameState == GameState::GAME_OVER) {
        gameOverTimer += deltaTime;

        if (gameOverTimer >= 5.0f) {
            manager.setGameState(GameState::MENU);
        }

        if (keys[SDL_SCANCODE_R]) {
            startMatch(manager.getGameMode());
        }
        return;
    }

    if (gameState != GameState::PLAYING) {
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

    if (manager.getGameMode() == GameMode::PVE) {
        updatePvEAI(deltaTime);
    } else {
        inputSys.update(*player2, deltaTime, keys);
    }
    ammoSys.update(*player2, deltaTime, bullets);

    if (player1->velocity.x > 1.0f) player1->facingRight = true;
    else if (player1->velocity.x < -1.0f) player1->facingRight = false;
    if (player2->velocity.x > 1.0f) player2->facingRight = true;
    else if (player2->velocity.x < -1.0f) player2->facingRight = false;

    physicsSys.updatePlayer(*player1, winds, deltaTime);
    physicsSys.updatePlayer(*player2, winds, deltaTime);
    physicsSys.updateBullets(bullets, winds, deltaTime);
    physicsSys.updateExplosions(explosions, deltaTime);

    collisionSys.update(*player1, bullets, platforms, items, explosions);
    collisionSys.update(*player2, bullets, platforms, items, explosions);

    if (player1->position.y > 630) player1->hp = 0;
    if (player2->position.y > 630) player2->hp = 0;

    if (player1->hp <= 0 || player2->hp <= 0) {
        manager.setGameState(GameState::GAME_OVER);
    }

    float rotSpeed = 360.0f / ROT_TIME;
    player1->aimAngle += rotSpeed * deltaTime;
    if (manager.getGameMode() == GameMode::PVP) {
        player2->aimAngle += rotSpeed * deltaTime;
    }
}

void Game::render() {
    if (!gameWindow) return;
    gameWindow->clear();
    SDL_Renderer* renderer = gameWindow->getRenderer();
    GameState currentState = GameManager::getInstance().getGameState(); 

    if (player1 && player2) {
        renderSys.render(renderer, *player1, *player2, bullets, platforms, items, winds, explosions,
                         texProjectile, texPlayer1, texPlayer2, texBackground, 
                         texWeapon1, texWeapon2,
                         texItemHealth, texItemMana, texItemShield, texBlast,
                         ResourceManager::getInstance());

        if (currentState == GameState::PLAYING) {
            float btnWidth = 40.0f;
            float pauseX = (SCREEN_WIDTH - btnWidth) * 0.5f;
            renderSys.renderPauseButton(renderer, SCREEN_WIDTH);
        }

        if (currentState == GameState::PAUSED) {
            renderSys.renderPauseMenu(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        if (currentState == GameState::GAME_OVER) {
             SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
             SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
             SDL_RenderFillRect(renderer, &overlay);
        }
    }
    
    gameWindow->display();
}

void Game::resetMatchEntities() {
    if (!player1 || !player2) return;

    player1->hp = player1->maxHp;
    player2->hp = player2->maxHp;
    player1->mana = player1->maxMana;
    player2->mana = player2->maxMana;
    player1->shieldTimer = 0.0f;
    player2->shieldTimer = 0.0f;
    player1->infiniteManaTimer = 0.0f;
    player2->infiniteManaTimer = 0.0f;
    player1->hitTimer = 0.0f;
    player2->hitTimer = 0.0f;
    player1->shootCooldown = 0.0f;
    player2->shootCooldown = 0.0f;
    player1->isCharging = false;
    player2->isCharging = false;
    player1->currentChargeTime = 0.0f;
    player2->currentChargeTime = 0.0f;
    player1->useUltimate = false;
    player2->useUltimate = false;
    player1->dropThroughTimer = 0.0f;
    player2->dropThroughTimer = 0.0f;
    player1->dropKeyHeld = false;
    player2->dropKeyHeld = false;
    isDraggingVolumeSlider = false;
    aiMoveSwitchTimer = 0.0f;
    aiJumpCooldownTimer = 0.0f;
    aiShootDecisionTimer = randomRange(0.2f, 0.5f);
    aiTargetChargeTime = randomRange(0.15f, 0.75f);
    aiUltimateDecisionTimer = randomRange(1.0f, 1.8f);
    aiMoveDirection = -1;

    player1->position.x = 332;
    player1->position.y = 100;
    player2->position.x = 1016;
    player2->position.y = 100;
    player1->previousY = player1->position.y;
    player2->previousY = player2->position.y;
    player1->velocity.x = 0;
    player1->velocity.y = 0;
    player2->velocity.x = 0;
    player2->velocity.y = 0;
    player1->aimAngle = 0.0f;
    player2->aimAngle = 180.0f;
    player1->facingRight = true;
    player2->facingRight = false;

    bullets.clear();
    items.clear();
    explosions.clear();
    itemSpawnTimer = 0.0f;
    gameOverTimer = 0.0f; 
}

void Game::startMatch(GameMode mode) {
    GameManager& manager = GameManager::getInstance();
    manager.setGameMode(mode);
    manager.resetGame();
    resetMatchEntities();
}

void Game::setMasterVolumeFromX(float mouseX) {
    float normalized = (mouseX - SETTINGS_SLIDER_X) / SETTINGS_SLIDER_W;
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    GameManager::getInstance().setMasterVolume(normalized);
    if (bgmStream) {
        SDL_SetAudioStreamGain(bgmStream, normalized);
    }
    if (battleBgmStream) {
        SDL_SetAudioStreamGain(battleBgmStream, normalized);
    }
}

bool Game::pointInRect(float x, float y, const SDL_FRect& rect) const {
    return x >= rect.x && x <= (rect.x + rect.w) && y >= rect.y && y <= (rect.y + rect.h);
}

void Game::updatePvEAI(float deltaTime) {
    if (!player1 || !player2) return;
    if (player1->hp <= 0 || player2->hp <= 0) return;

    aiMoveSwitchTimer -= deltaTime;
    aiJumpCooldownTimer -= deltaTime;
    aiShootDecisionTimer -= deltaTime;
    aiUltimateDecisionTimer -= deltaTime;

    float p1CenterX = player1->position.x + player1->width * 0.5f;
    float p1CenterY = player1->position.y + player1->height * 0.5f;
    float p2CenterX = player2->position.x + player2->width * 0.5f;
    float p2CenterY = player2->position.y + player2->height * 0.5f;
    float dx = p1CenterX - p2CenterX;
    float dy = p1CenterY - p2CenterY;
    float absDx = std::fabs(dx);

    bool incomingThreat = false;
    for (const auto& bullet : bullets) {
        if (!bullet.active || bullet.ownerId != 1 || bullet.level == 4) continue;

        float bulletCenterX = bullet.position.x + bullet.radius;
        float bulletCenterY = bullet.position.y + bullet.radius;
        float toBotX = p2CenterX - bulletCenterX;
        float toBotY = p2CenterY - bulletCenterY;
        float dot = bullet.velocity.x * toBotX + bullet.velocity.y * toBotY;

        if (dot > 0.0f && std::fabs(toBotY) < 92.0f && std::fabs(toBotX) < 260.0f) {
            incomingThreat = true;
            break;
        }
    }

    if (aiMoveSwitchTimer <= 0.0f) {
        if (absDx > 90.0f) {
            aiMoveDirection = (dx > 0.0f) ? 1 : -1;
        } else {
            int roll = rand() % 100;
            if (roll < 35) aiMoveDirection = (dx > 0.0f) ? -1 : 1;
            else if (roll < 70) aiMoveDirection = (dx > 0.0f) ? 1 : -1;
            else aiMoveDirection = 0;
        }
        aiMoveSwitchTimer = randomRange(0.18f, 0.48f);
    }

    if (player2->position.x < 70.0f) aiMoveDirection = 1;
    if (player2->position.x > SCREEN_WIDTH - player2->width - 70.0f) aiMoveDirection = -1;
    player2->velocity.x = aiMoveDirection * (MOVE_SPEED * 0.86f);

    bool shouldJumpToAttack = (dy < -65.0f && absDx < 280.0f);
    if (player2->isGrounded && aiJumpCooldownTimer <= 0.0f && (incomingThreat || shouldJumpToAttack)) {
        player2->velocity.y = JUMP_FORCE;
        player2->isGrounded = false;
        aiJumpCooldownTimer = incomingThreat ? randomRange(0.85f, 1.25f) : randomRange(0.95f, 1.45f);
    }

    float predictedX = p1CenterX + player1->velocity.x * 0.22f + randomRange(-18.0f, 18.0f);
    float predictedY = p1CenterY + player1->velocity.y * 0.10f + randomRange(-11.0f, 11.0f);
    float targetAngle = std::atan2(predictedY - p2CenterY, predictedX - p2CenterX) * (180.0f / PI);
    float delta = shortestAngleDelta(player2->aimAngle, targetAngle);
    float maxTurnStep = 280.0f * deltaTime;
    if (delta > maxTurnStep) delta = maxTurnStep;
    if (delta < -maxTurnStep) delta = -maxTurnStep;
    player2->aimAngle += delta;

    if (player2->mana >= 100.0f && player2->shootCooldown <= 0.0f && !player2->isCharging && aiUltimateDecisionTimer <= 0.0f) {
        if (absDx < 560.0f && (rand() % 100) < 24) {
            player2->useUltimate = true;
            aiShootDecisionTimer = randomRange(0.65f, 1.05f);
        }
        aiUltimateDecisionTimer = randomRange(1.2f, 2.2f);
    }

    if (player2->shootCooldown <= 0.0f && player2->mana >= MANA_COST) {
        if (!player2->isCharging && aiShootDecisionTimer <= 0.0f && absDx < 760.0f) {
            player2->isCharging = true;
            aiTargetChargeTime = randomRange(0.16f, 0.78f);
            aiShootDecisionTimer = randomRange(0.55f, 1.15f);
        }

        if (player2->isCharging) {
            bool releaseForThreat = incomingThreat && absDx < 300.0f && player2->currentChargeTime > 0.15f;
            if (player2->currentChargeTime >= aiTargetChargeTime || releaseForThreat) {
                player2->isCharging = false;
            }
        }
    } else if (player2->isCharging) {
        player2->isCharging = false;
    }
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
    
    if (battleBgmStream) {
        SDL_DestroyAudioStream(battleBgmStream);
        battleBgmStream = nullptr;
    }
    if (battleBgmAudioData) {
        SDL_free(battleBgmAudioData);
        battleBgmAudioData = nullptr;
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