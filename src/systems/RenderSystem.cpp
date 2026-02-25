#include "../../include/systems/RenderSystem.h"
#include "../../include/utils/Constants.h"
#include <SDL3/SDL_ttf.h>
#include <cmath>
#include <string>

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
        if (err <= 0) { y += 1; err += 2 * y + 1; }
        if (err > 0) { x -= 1; err -= 2 * x + 1; }
    }
}

void RenderSystem::render(SDL_Renderer* renderer, 
                          const Player& p1, const Player& p2,
                          const std::vector<Projectile>& bullets, 
                          const std::vector<Platform>& platforms, 
                          const std::vector<Item>& items,
                          const std::vector<WindColumn>& winds,
                          const std::vector<Explosion>& explosions,
                          SDL_Texture* texBullet, SDL_Texture* texP1, SDL_Texture* texP2, 
                          SDL_Texture* texBG, 
                          SDL_Texture* texW1, SDL_Texture* texW2,
                          SDL_Texture* texItemHealth, SDL_Texture* texItemMana, SDL_Texture* texItemShield,
                          SDL_Texture* texBlast,
                          ResourceManager& rm)
{
    if (texBG) {
        SDL_RenderTexture(renderer, texBG, NULL, NULL); 
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
    }

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
        for (int i = 0; i < numStreaks; ++i) {
            float streakX = rect.x + (i * (rect.w / numStreaks)) + 10.0f;
            float offset = fmod((ticks * 0.3f + i * 100.0f), rect.h);
            float streakY = rect.y + rect.h - offset;
            SDL_FRect streak = { streakX, streakY, 4.0f, 30.0f };
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
        SDL_Texture* iconTex = (item.type == ITEM_HEAL) ? texItemHealth : (item.type == ITEM_SHIELD ? texItemShield : texItemMana);
        SDL_Color frameColor = (item.type == ITEM_HEAL) ? SDL_Color{50, 255, 50, 255} : (item.type == ITEM_SHIELD ? SDL_Color{50, 255, 255, 255} : SDL_Color{255, 200, 50, 255});
        drawFilledCircle(renderer, cx, cy, radius, frameColor);
        drawFilledCircle(renderer, cx, cy, radius - 2.0f, {255, 255, 255, 255});
        if (iconTex) SDL_RenderTexture(renderer, iconTex, NULL, &rect);
    }

    renderPlayer(renderer, p1, texP1, texP2, texW1, texW2);
    renderPlayer(renderer, p2, texP1, texP2, texW1, texW2);

    for (const auto& p : bullets) {
        if (!p.active) continue;
        SDL_FRect dst = {p.position.x, p.position.y, p.radius * 2.0f, p.radius * 2.0f};
        std::string key;
        if (p.level == 4) {
            key = (p.ownerId == 1) ? "Ultimate_P1" : "Ultimate_P2";
            if (p.ownerId == 1) {
                dst.w = p.radius * 4.5f; 
                dst.h = p.radius * 2.8f;
            } else {
                dst.w = p.radius * 6.5f; 
                dst.h = p.radius * 2.8f;
            }
        } else {
            key = (p.ownerId == 1 ? "Dark_" : "Nebula_") + std::to_string(p.level);
        }

        SDL_Texture* tex = rm.getTexture(key);
        if (tex) {
            float texW, texH;
            SDL_GetTextureSize(tex, &texW, &texH);
            float frameWidth = texW / (float)p.maxFrames;
            float frameHeight = texH;
            float angle = std::atan2(p.velocity.y, p.velocity.x) * (180.0f / 3.14159f);
            SDL_FRect srcRect = { (float)p.currentFrame * frameWidth, 0.0f, frameWidth, frameHeight };
            SDL_RenderTextureRotated(renderer, tex, &srcRect, &dst, angle, NULL, SDL_FLIP_NONE);
        }
    }

    renderExplosions(renderer, explosions, texBlast);
    renderUI(renderer, p1, p2);
    if (p1.hp <= 0 || p2.hp <= 0) renderGameOver(renderer, p1, p2);
}

void RenderSystem::renderExplosions(SDL_Renderer* renderer, const std::vector<Explosion>& explosions, SDL_Texture* texBlast) {
    if (!texBlast) return;

    for (const auto& exp : explosions) {
        if (!exp.active) continue;

        SDL_FRect srcRect = { (float)(exp.currentFrame * 70), 0.0f, 70.0f, 70.0f };
        
        float displaySize = 280.0f; 
        float offset = displaySize / 2.0f; 
        SDL_FRect dstRect = { 
            exp.x - offset, 
            exp.y - offset, 
            displaySize, 
            displaySize 
        };
        
        SDL_RenderTexture(renderer, texBlast, &srcRect, &dstRect);
    }
}

void RenderSystem::renderPlayer(SDL_Renderer* renderer, const Player& player, SDL_Texture* texP1, SDL_Texture* texP2, SDL_Texture* texW1, SDL_Texture* texW2) {
    if (player.hp <= 0) return;
    SDL_FRect rect = player.getRect();
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;

    if (player.isCharging) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        Uint64 ticks = SDL_GetTicks();
        for (int i = 0; i < 3; i++) {
            float progress = fmod((ticks * 0.002f + i * 0.33f), 1.0f); 
            float radius = 60.0f * (1.0f - progress); 
            SDL_Color chargeColor = (player.id == 1) ? SDL_Color{255, 200, 50, (Uint8)(progress * 150)} : SDL_Color{200, 100, 255, (Uint8)(progress * 150)};
            SDL_SetRenderDrawColor(renderer, chargeColor.r, chargeColor.g, chargeColor.b, chargeColor.a);
            for (float angle = 0; angle < 360; angle += 5) {
                float rad = angle * PI / 180.0f;
                SDL_RenderPoint(renderer, cx + cos(rad) * radius, cy + sin(rad) * radius);
            }
        }
    }

    if (player.shieldTimer > 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 50, 255, 255, 120); 
        SDL_FRect sRect = {rect.x - 10, rect.y - 10, rect.w + 20, rect.h + 20};
        SDL_RenderFillRect(renderer, &sRect);
    }

    SDL_Texture* currentTex = (player.id == 1) ? texP1 : texP2;
    if (currentTex) {
        SDL_FlipMode flip = (player.aimAngle > 90 && player.aimAngle < 270) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        if (player.hitTimer > 0) SDL_SetTextureColorMod(currentTex, 255, 100, 100);
        else SDL_SetTextureColorMod(currentTex, 255, 255, 255);
        SDL_RenderTextureRotated(renderer, currentTex, NULL, &rect, 0, NULL, flip);
    }

    renderSideManaBar(renderer, player);

    SDL_Texture* currentWeaponTex = (player.id == 1) ? texW1 : texW2;
    if (currentWeaponTex) {
        float wWidth, wHeight;
        SDL_GetTextureSize(currentWeaponTex, &wWidth, &wHeight);
        float rW = wWidth / 3.0f, rH = wHeight / 3.0f;
        SDL_FRect weaponRect = { cx, cy - rH / 2, rW, rH };
        SDL_FPoint center = { 0.0f, rH / 2 };
        float renderAngle = player.aimAngle + 90.0f;
        SDL_FlipMode wFlip = (player.aimAngle > 90 && player.aimAngle < 270) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        if (player.hitTimer > 0) SDL_SetTextureColorMod(currentWeaponTex, 255, 100, 100);
        else SDL_SetTextureColorMod(currentWeaponTex, 255, 255, 255);
        SDL_RenderTextureRotated(renderer, currentWeaponTex, NULL, &weaponRect, renderAngle, &center, wFlip);
    }
}

void RenderSystem::renderSideManaBar(SDL_Renderer* renderer, const Player& player) {
    float barWidth = 6.0f;
    float xPos = (player.id == 1) ? (player.position.x - 14.0f) : (player.position.x + player.width + 8.0f);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200);
    SDL_FRect bg = { xPos, player.position.y, barWidth, player.height };
    SDL_RenderFillRect(renderer, &bg);
    float mRatio = player.mana / player.maxMana;
    SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
    SDL_FRect fill = { xPos, player.position.y + (player.height * (1 - mRatio)), barWidth, player.height * mRatio };
    SDL_RenderFillRect(renderer, &fill);
}

void RenderSystem::renderUI(SDL_Renderer* renderer, const Player& p1, const Player& p2) {
    float barW = 300.0f, hpH = 25.0f, offset = 20.0f;
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_FRect bg1 = { offset, offset, barW, hpH }, bg2 = { SCREEN_WIDTH - barW - offset, offset, barW, hpH };
    SDL_RenderFillRect(renderer, &bg1); SDL_RenderFillRect(renderer, &bg2);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect hp1 = { offset, offset, (p1.hp / p1.maxHp) * barW, hpH };
    SDL_FRect hp2 = { SCREEN_WIDTH - offset - ((p2.hp / p2.maxHp) * barW), offset, (p2.hp / p2.maxHp) * barW, hpH };
    SDL_RenderFillRect(renderer, &hp1); SDL_RenderFillRect(renderer, &hp2);
}

void RenderSystem::renderGameOver(SDL_Renderer* renderer, const Player& p1, const Player& p2) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 210);
    SDL_FRect overlay = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);
    TTF_Font* font = TTF_OpenFont("assets/fonts/LibreBaskerville-Italic-VariableFont_wght.ttf", 90);
    if (font) {
        std::string winTxt = (p1.hp <= 0) ? "PLAYER 2 WINS!" : "PLAYER 1 WINS!";
        SDL_Color clr = (p1.hp <= 0) ? SDL_Color{255, 100, 100, 255} : SDL_Color{100, 200, 255, 255};
        SDL_Surface* surf = TTF_RenderText_Blended(font, winTxt.c_str(), 0, clr);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            float w = (float)surf->w, h = (float)surf->h;
            SDL_FRect dst = { (SCREEN_WIDTH - w) / 2.0f, (SCREEN_HEIGHT - h) / 2.0f - 40, w, h };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
            SDL_DestroySurface(surf); SDL_DestroyTexture(tex);
        }
        TTF_CloseFont(font);
    }
}