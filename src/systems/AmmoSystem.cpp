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
            float spawnX = (centerX + std::cos(rad) * offset/2) - bulletRadius;
            float spawnY = (centerY + std::sin(rad) * offset/2) - bulletRadius;

            int myBullets = 0;
            for(auto& b : bullets) if(b.ownerId == player.id && b.active) myBullets++;

            if (myBullets < MAX_PROJECTILES) {
                bullets.push_back(Projectile(spawnX, spawnY, velX, velY, bulletRadius, player.id, damage));
                player.mana -= MANA_COST;
                player.shootCooldown = 0.2f;
            }
        }
        player.currentChargeTime = 0.0f;
    }
}