#include "entities/Zombie.hpp"

Zombie::Zombie(int x, int y, int w, int h, float speed, int hp)
    : Entity(x, y, w, h), speed_(speed), hp_(hp) {}

void Zombie::update(float dt) {
  if (!attacking_) {
    rect_.x -= static_cast<int>(speed_ * dt);
    attackTimer_ = 0.0;
  } else {
    attackTimer_ += dt;
  };
}

void Zombie::takeDamage(int dmg) { hp_ -= dmg; }

void Zombie::render(SDL_Renderer *renderer) {
  int intesity = std::max(0, 100 + hp_ * 40);
  SDL_SetRenderDrawColor(renderer, intesity, 0, 0, 255); // Darker red for damaged zombies
  SDL_RenderFillRect(renderer, &rect_);
}