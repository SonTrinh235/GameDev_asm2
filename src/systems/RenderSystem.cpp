#include "../../include/systems/RenderSystem.h"
#include "../../include/utils/Constants.h"
#include <cmath>

void RenderSystem::drawFilledCircle(SDL_Renderer* renderer, float cx, float cy, float radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    int x = (int)radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderLine(renderer, cx - x, cy - y, cx + x, cy - y);
        SDL_RenderLine(renderer, cx - x, cy + y, cx + x, cy + y);
        SDL_RenderLine(renderer, cx - y, cy - x, cx + y, cy - x);
        SDL_RenderLine(renderer, cx - y, cy + x, cx + y, cy + x);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void RenderSystem::render(SDL_Renderer* renderer, 
                          const Player& p1, const Player& p2,
                          const std::vector<Projectile>& bullets, 
                          const std::vector<Platform>& platforms, 
                          const std::vector<Item>& items,
                          const std::vector<WindColumn>& winds,
                          SDL_Texture* texBullet, SDL_Texture* texP1, SDL_Texture* texP2, 
                          SDL_Texture* texBG, 
                          SDL_Texture* texW1, SDL_Texture* texW2,
                          SDL_Texture* texItemHealth, SDL_Texture* texItemMana, SDL_Texture* texItemShield) // Nhận thêm 3 texture
{
    if (texBG) {
        SDL_RenderTexture(renderer, texBG, NULL, NULL); 
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
    }

    // Platforms
    for (const auto& plat : platforms) {
        SDL_FRect rect = plat.getRect();
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 180); 
        SDL_RenderFillRect(renderer, &rect);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    Uint64 ticks = SDL_GetTicks(); 

    for (const auto& wind : winds) {
        SDL_FRect rect = wind.getRect();

        SDL_SetRenderDrawColor(renderer, 150, 255, 255, 40);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 200, 255, 255, 150); 
        
        int numStreaks = 6; 
        float streakWidth = 4.0f; 
        float streakHeight = 30.0f; 

        for (int i = 0; i < numStreaks; ++i) {
            float streakX = rect.x + (i * (rect.w / numStreaks)) + 10.0f;
            float offset = fmod((ticks * 0.3f + i * 100.0f), rect.h);
            float streakY = rect.y + rect.h - offset;

            SDL_FRect streak = { streakX, streakY, streakWidth, streakHeight };
            SDL_RenderFillRect(renderer, &streak);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    for (const auto& item : items) {
        if (!item.active) continue;
        SDL_FRect rect = item.getRect();
        
        float cx = rect.x + rect.w / 2.0f;
        float cy = rect.y + rect.h / 2.0f;
        float radius = rect.w / 2.0f + 5.0f;

        SDL_Texture* iconTex = nullptr;
        SDL_Color frameColor = {255, 255, 255, 255};

        if (item.type == ITEM_HEAL) {
            frameColor = {50, 255, 50, 255};
            iconTex = texItemHealth;
        } else if (item.type == ITEM_SHIELD) {
            frameColor = {50, 255, 255, 255};
            iconTex = texItemShield;
        } else if (item.type == ITEM_INFINITE_MANA) {
            frameColor = {255, 200, 50, 255};
            iconTex = texItemMana;
        }

        drawFilledCircle(renderer, cx, cy, radius, frameColor);

        drawFilledCircle(renderer, cx, cy, radius - 2.0f, {255, 255, 255, 255});

        if (iconTex) {
            SDL_RenderTexture(renderer, iconTex, NULL, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, frameColor.r, frameColor.g, frameColor.b, frameColor.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    // Players
    renderPlayer(renderer, p1, texP1, texP2, texW1, texW2);
    renderPlayer(renderer, p2, texP1, texP2, texW1, texW2);

    // Projectiles
    for (const auto& p : bullets) {
        if (!p.active) continue;
        SDL_FRect dst = {p.position.x, p.position.y, p.radius*2, p.radius*2};
        if (texBullet) {
            SDL_RenderTexture(renderer, texBullet, NULL, &dst);
        } else {
            if (p.ownerId == 1) SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            else SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            SDL_RenderFillRect(renderer, &dst);
        }
    }

    // HUD
    renderUI(renderer, p1, p2);

    // Game Over
    if (p1.hp <= 0 || p2.hp <= 0) {
        renderGameOver(renderer, p1, p2);
    }
}

void RenderSystem::renderPlayer(SDL_Renderer* renderer, const Player& player, SDL_Texture* texP1, SDL_Texture* texP2, SDL_Texture* texW1, SDL_Texture* texW2) {
    if (player.hp <= 0) return;

    SDL_FRect rect = player.getRect();

    if (player.shieldTimer > 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 50, 255, 255, 120); 
        SDL_FRect sRect = {rect.x - 10, rect.y - 10, rect.w + 20, rect.h + 20};
        SDL_RenderFillRect(renderer, &sRect);
    }

    SDL_Texture* currentTex = (player.id == 1) ? texP1 : texP2;

    if (currentTex != nullptr) {
        SDL_FlipMode flip = SDL_FLIP_NONE;
        if (player.aimAngle > 90 && player.aimAngle < 270) flip = SDL_FLIP_HORIZONTAL;
        
        if (player.isCharging) {
            float ratio = player.currentChargeTime / MAX_CHARGE_TIME;
            Uint8 boost = (Uint8)(ratio * 150);
            
            if (player.id == 1) {
                SDL_SetTextureColorMod(currentTex, 255 - boost, 255, 255); 
            } else {
                SDL_SetTextureColorMod(currentTex, 255, 255 - boost, 255 - boost); 
            }
        } else {
            SDL_SetTextureColorMod(currentTex, 255, 255, 255);
        }
        SDL_RenderTextureRotated(renderer, currentTex, NULL, &rect, 0, NULL, flip);
    } else {
        SDL_SetRenderDrawColor(renderer, player.r, player.g, player.b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Mana
    renderSideManaBar(renderer, player);

    // Weapon
    SDL_Texture* currentWeaponTex = (player.id == 1) ? texW1 : texW2;
    if (currentWeaponTex) {
        float wWidth, wHeight;
        SDL_GetTextureSize(currentWeaponTex, &wWidth, &wHeight);

        float renderWidth = wWidth / 3.0f;
        float renderHeight = wHeight / 3.0f;

        float cx = player.position.x + player.width / 2;
        float cy = player.position.y + player.height / 2;

        SDL_FRect weaponRect = { cx, cy - renderHeight / 2, renderWidth, renderHeight };
        SDL_FPoint center = { 0.0f, renderHeight / 2 };

        SDL_FlipMode weaponFlip = SDL_FLIP_NONE;
        
        float rotationOffset = 90.0f; 
        float renderAngle = player.aimAngle + rotationOffset;

        if (player.aimAngle > 90 && player.aimAngle < 270) {
            weaponFlip = SDL_FLIP_VERTICAL;
        }

        SDL_RenderTextureRotated(renderer, currentWeaponTex, NULL, &weaponRect, renderAngle, &center, weaponFlip);
    }
}

void RenderSystem::renderSideManaBar(SDL_Renderer* renderer, const Player& player) {
    float barWidth = 6.0f;
    float barHeight = player.height;
    float spacing = 8.0f;
    
    float xPos = (player.id == 1) ? (player.position.x - spacing - barWidth) : (player.position.x + player.width + spacing);
    float yPos = player.position.y;

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200);
    SDL_FRect bg = { xPos, yPos, barWidth, barHeight };
    SDL_RenderFillRect(renderer, &bg);

    float manaRatio = player.mana / player.maxMana;
    float currentManaHeight = barHeight * manaRatio;
    
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
    SDL_FRect fill = { xPos, yPos + (barHeight - currentManaHeight), barWidth, currentManaHeight };
    SDL_RenderFillRect(renderer, &fill);
}

void RenderSystem::renderUI(SDL_Renderer* renderer, const Player& p1, const Player& p2) {
    float barW = 300.0f;
    float hpH = 25.0f;
    float offset = 20.0f;

    // P1 Health
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_FRect bg1 = { offset, offset, barW, hpH };
    SDL_RenderFillRect(renderer, &bg1);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect hp1 = { offset, offset, (p1.hp / p1.maxHp) * barW, hpH }; 
    SDL_RenderFillRect(renderer, &hp1);

    // P2 Health
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_FRect bg2 = { SCREEN_WIDTH - barW - offset, offset, barW, hpH };
    SDL_RenderFillRect(renderer, &bg2);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    float p2HpW = (p2.hp / p2.maxHp) * barW;
    SDL_FRect hp2 = { SCREEN_WIDTH - offset - p2HpW, offset, p2HpW, hpH };
    SDL_RenderFillRect(renderer, &hp2);
}

void RenderSystem::renderGameOver(SDL_Renderer* renderer, const Player& p1, const Player& p2) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlay = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    float boxW = 300.0f, boxH = 100.0f;
    SDL_FRect winBox = { (SCREEN_WIDTH - boxW) / 2, (SCREEN_HEIGHT - boxH) / 2, boxW, boxH };

    if (p1.hp <= 0) SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255); 
    else SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); 
    
    SDL_RenderFillRect(renderer, &winBox);
}