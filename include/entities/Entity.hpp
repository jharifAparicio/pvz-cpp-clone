#pragma once
#include <SDL.h>

class Entity {
public:
    Entity(int x, int y, int w, int h);
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer);

    SDL_Rect getRect() const { return rect_; }

protected:
    SDL_Rect rect_;
};