#pragma once
#include "entities/Entity.hpp"

class Projectile : public Entity {
public:
    Projectile(int x, int y, int w, int h, float speed);
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    float speed_;
};
