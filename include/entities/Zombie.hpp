#pragma once
#include "entities/Entity.hpp"

class Zombie : public Entity {
public:
    Zombie(int x, int y, int w, int h, float speed, int hp_ = 3);
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    void takeDamage(int dmg);
    bool isDead() const { return hp_ <= 0; }

    void stop() { speed_ = 0.0f; }
    void resume() { speed_ = baseSpeed_; }

    bool isAttacking() const { return attacking_; }
    void setAttacking(bool val) {attacking_ = val; };

private:
    float speed_ = 0.0f;
    float baseSpeed_ = 0.0f;
    int hp_ = 3;
    bool attacking_ = false;
    double attackTimer_ = 0.0;
    double attackCooldown_ = 1.0; // muerde cada segundo
};
