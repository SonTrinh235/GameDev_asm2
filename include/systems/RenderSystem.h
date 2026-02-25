#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "../entities/Player.h"
#include "../entities/Projectile.h"
#include "../entities/Platform.h"
#include "../entities/Item.h"
#include "../entities/WindColumn.h"
#include "../managers/ResourceManager.h"
#include "CollisionSystem.h"

class RenderSystem {
public:
    void render(SDL_Renderer* renderer, 
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
                ResourceManager& rm);

private:
    void renderPlayer(SDL_Renderer* renderer, const Player& player, SDL_Texture* texP1, SDL_Texture* texP2, SDL_Texture* texW1, SDL_Texture* texW2);
    void renderSideManaBar(SDL_Renderer* renderer, const Player& player);
    void renderUI(SDL_Renderer* renderer, const Player& p1, const Player& p2);
    void renderGameOver(SDL_Renderer* renderer, const Player& p1, const Player& p2);
    void renderExplosions(SDL_Renderer* renderer, const std::vector<Explosion>& explosions, SDL_Texture* texBlast);
    void drawFilledCircle(SDL_Renderer* renderer, float cx, float cy, float radius, SDL_Color color);
};