#include "entities/Plant.hpp"
#include <SDL.h>
#include <algorithm>

Plant::Plant(int x, int y, int w, int h) : Entity(x, y, w, h) {}

void Plant::update(float dt) {
  shootTimer_ += dt; // suposición de ~60 FPS para simplificar
}

void Plant::render(SDL_Renderer *renderer) {
  int intensity = std::max(0, 50 + static_cast<int>(hp_ * 40));
  SDL_SetRenderDrawColor(renderer, 0, intensity, 0, 255); // Green color for plants
  SDL_RenderFillRect(renderer, &rect_);
}

void Plant::takeDamage(int dmg) { hp_ -= dmg; }