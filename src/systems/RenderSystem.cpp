#include "../../include/systems/RenderSystem.h"
#include "../../include/utils/Constants.h"
#include <cmath>

void RenderSystem::render(SDL_Renderer* renderer, 
                          const Player& p1, const Player& p2,
                          const std::vector<Projectile>& bullets, 
                          const std::vector<Platform>& platforms, 
                          SDL_Texture* texBullet, SDL_Texture* texP1, SDL_Texture* texP2, 
                          SDL_Texture* texBG, 
                          SDL_Texture* texW1, SDL_Texture* texW2) 
{
    if (texBG) {
        SDL_RenderTexture(renderer, texBG, NULL, NULL); 
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
    }

    // Platforms
    // for (const auto& plat : platforms) {
    //     SDL_FRect rect = plat.getRect();
    //     SDL_SetRenderDrawColor(renderer, 100, 100, 100, 180); 
    //     SDL_RenderFillRect(renderer, &rect);
    // }

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

    //HUD
    renderUI(renderer, p1, p2);

    //Game Over
    if (p1.hp <= 0 || p2.hp <= 0) {
        renderGameOver(renderer, p1, p2);
    }
}

void RenderSystem::renderPlayer(SDL_Renderer* renderer, const Player& player, SDL_Texture* texP1, SDL_Texture* texP2, SDL_Texture* texW1, SDL_Texture* texW2) {
    if (player.hp <= 0) return;

    SDL_FRect rect = player.getRect();
    SDL_Texture* currentTex = (player.id == 1) ? texP1 : texP2;

    if (currentTex != nullptr) {
        SDL_FlipMode flip = SDL_FLIP_NONE;
        if (player.aimAngle > 90 && player.aimAngle < 270) flip = SDL_FLIP_HORIZONTAL;
        
        if (player.isCharging) {
            float ratio = player.currentChargeTime / MAX_CHARGE_TIME;
            Uint8 boost = (Uint8)(ratio * 150);
            SDL_SetTextureColorMod(currentTex, 255, 255 - boost, 255 - boost); 
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