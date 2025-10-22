#pragma once
#include <SDL.h>
#include <string>

class Game {
public:
    Game() = default;
    ~Game();

    bool init(const std::string& title, int width, int height, bool vsync = true);
    void run();       // Bucle principal
    void shutdown();

private:
    void processEvents(bool& running);
    void update(float dt);
    void render();

private:
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    int width_  = 1280;
    int height_ = 720;
    bool vsync_ = true;

    // timing
    Uint64 lastCounter_ = 0;
    double perfFreq_    = 0.0;
};
