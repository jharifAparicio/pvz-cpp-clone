#include "core/Game.hpp"
#include <iostream>

Game::~Game() { 
    shutdown(); 
}

bool Game::init(const std::string& title, int width, int height, bool vsync) {
    width_ = width;
    height_ = height;
    vsync_ = vsync;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window_ = SDL_CreateWindow(title.c_str(),
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width_, height_, SDL_WINDOW_SHOWN);
    if (!window_) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    Uint32 renderFlags = SDL_RENDERER_ACCELERATED | (vsync_ ? SDL_RENDERER_PRESENTVSYNC : 0);
    renderer_ = SDL_CreateRenderer(window_, -1, renderFlags);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    perfFreq_ = static_cast<double>(SDL_GetPerformanceFrequency());
    lastCounter_ = SDL_GetPerformanceCounter();
    return true;
}

void Game::run() {
    bool running = true;
    const double targetFrame = 1.0 / 60.0;
    double lag = 0.0;

    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        double delta = (now - lastCounter_) / perfFreq_;
        lastCounter_ = now;
        lag += delta;

        processEvents(running);

        while (lag >= targetFrame) {
            update(static_cast<float>(targetFrame));
            lag -= targetFrame;
        }

        render();
    }
}

void Game::processEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
    }
}

void Game::update(float /*dt*/) {
    // Lógica futura del juego (planta, zombie, disparo...)
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer_, 90, 170, 90, 255);
    SDL_RenderClear(renderer_);

    // Césped simulado (5 carriles)
    SDL_SetRenderDrawColor(renderer_, 60, 120, 60, 255);
    for (int i = 1; i <= 4; ++i) {
        int y = (height_ / 5) * i;
        SDL_Rect line{0, y, width_, 3};
        SDL_RenderFillRect(renderer_, &line);
    }

    SDL_RenderPresent(renderer_);
}

void Game::shutdown() {
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_)   { SDL_DestroyWindow(window_); window_ = nullptr; }
    SDL_Quit();
}
