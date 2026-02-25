#include "../../include/systems/AmmoSystem.h"
#include "../../include/utils/Constants.h"
#include <cmath>

void AmmoSystem::update(Player& player, float deltaTime, std::vector<Projectile>& bullets) {
    if (player.hp <= 0) return;

    if (player.mana < player.maxMana) {
        player.mana += MANA_REGEN * deltaTime;
        if (player.mana > player.maxMana) player.mana = player.maxMana;
    }

    if (player.shootCooldown > 0) player.shootCooldown -= deltaTime;

    if (player.useUltimate) {
        if (player.mana >= 100.0f && player.shootCooldown <= 0) {
            float centerX = player.position.x + player.width / 2;
            float centerY = player.position.y + player.height / 2;

            const bool* keys = SDL_GetKeyboardState(NULL);
            float direction = 0.0f;

            if (player.id == 1) {
                if (keys[SDL_SCANCODE_A]) direction = -1.0f;
                else if (keys[SDL_SCANCODE_D]) direction = 1.0f;
            } else {
                if (keys[SDL_SCANCODE_LEFT]) direction = -1.0f;
                else if (keys[SDL_SCANCODE_RIGHT]) direction = 1.0f;
            }

            if (direction == 0.0f) {
                float angle = player.aimAngle;
                while (angle < 0) angle += 360.0f;
                while (angle >= 360.0f) angle -= 360.0f;

                if (angle > 90.0f && angle < 270.0f) {
                    direction = -1.0f;
                } else {
                    direction = 1.0f;
                }
            }

            float velX = direction * (BULLET_SPEED * 1.2f);
            float velY = 0; 

            bullets.push_back(Projectile(centerX, centerY, velX, velY, player.id, 50.0f, 45.0f, 4));

            player.mana -= 100.0f;
            player.shootCooldown = 1.5f; 
        }
        
        player.useUltimate = false;
        player.isCharging = false;
        player.currentChargeTime = 0.0f;
        return;
    }

    if (player.isCharging) {
        player.currentChargeTime += deltaTime;
        if (player.currentChargeTime > MAX_CHARGE_TIME) player.currentChargeTime = MAX_CHARGE_TIME;
    }

    if (!player.isCharging && player.currentChargeTime > 0) {
        if (player.mana >= MANA_COST && player.shootCooldown <= 0) {
        // if (player.mana >= (MANA_COST + 0.5 * deltaTime)  && player.shootCooldown <= 0) {
            float ratio = player.currentChargeTime / MAX_CHARGE_TIME;
            float bulletRadius = MIN_BULLET_RADIUS + (MAX_BULLET_RADIUS - MIN_BULLET_RADIUS) * ratio;
            float damage = 10.0f + (ratio * 20.0f); 
            // float totalManaConsume = MANA_COST + 2 * deltaTime;
            
            float rad = player.aimAngle * (PI / 180.0f);
            float velX = std::cos(rad) * BULLET_SPEED;
            float velY = std::sin(rad) * BULLET_SPEED;
            
            float centerX = player.position.x + player.width / 2;
            float centerY = player.position.y + player.height / 2;
            float offset = player.width > player.height ? player.width : player.height;
            
            float spawnDist = offset / 2.0f + bulletRadius + 5.0f;
            float spawnX = centerX + std::cos(rad) * spawnDist;
            float spawnY = centerY + std::sin(rad) * spawnDist;

            int myBullets = 0;
            for(auto& b : bullets) if(b.ownerId == player.id && b.active) myBullets++;

            if (myBullets < MAX_PROJECTILES) {
                int bulletLevel = 1;
                if (ratio >= 0.8f) bulletLevel = 3;
                else if (ratio >= 0.4f) bulletLevel = 2;

                bullets.push_back(Projectile(spawnX, spawnY, velX, velY, player.id, bulletRadius, damage, bulletLevel));
                
                if (ratio > 0.2f) {
                    float recoilMag = ratio * 1000.0f; 
                    player.velocity.x -= std::cos(rad) * recoilMag;
                    player.velocity.y -= std::sin(rad) * recoilMag;
                    player.position.y -= 5.0f; 
                    player.isGrounded = false;
                    if (std::abs(std::sin(rad)) < 0.3f) player.velocity.y -= 200.0f; 
                }

                player.mana -= MANA_COST;
                player.shootCooldown = 0.2f;
            }
        }
        player.currentChargeTime = 0.0f;
    }
}