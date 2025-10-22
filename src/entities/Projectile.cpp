#include "entities/Projectile.hpp"

Projectile::Projectile(int x, int y, int w, int h, float speed)
    : Entity(x, y, w, h), speed_(speed) {}

void Projectile::update(float dt) {
    rect_.x += static_cast<int>(speed_ * dt);
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 50, 150); // Yellow color for projectiles
    SDL_RenderFillRect(renderer, &rect_);
}