#include "entities/Entity.hpp"

Entity::Entity(int x, int y, int w, int h) { 
  rect_ = {x, y, w, h}; 
}

void Entity::render(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for default
  SDL_RenderFillRect(renderer, &rect_);
}