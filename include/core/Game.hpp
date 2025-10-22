#pragma once
#include "core/Grid.hpp"
#include "entities/Entity.hpp"
#include "entities/Plant.hpp"
#include "entities/Projectile.hpp"
#include "entities/Zombie.hpp"
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>

class Game {
public:
  Game() = default;
  ~Game();

  bool init(const std::string &title, int width, int height, bool vsync = true);
  void run(); // Bucle principal
  void shutdown();

private:
  void processEvents(bool &running);
  void update(float dt);
  void render();
  bool checkCollision(const SDL_Rect &a, const SDL_Rect &b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
  }

private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  int width_ = 1280;
  int height_ = 720;
  bool vsync_ = true;

  // timing
  Uint64 lastCounter_ = 0;
  double perfFreq_ = 0.0;

private:
  std::unique_ptr<Grid> grid_;
  std::vector<std::unique_ptr<Entity>> entities_;

  double projectileCooldown_ = 1.5;
  double projectileTimer_ = 0.0;

  double zombieTimer_ = 0.0;
  double zombieCooldown_ = 5.0;

  bool isCellEmpty(int row, int col) const;
  SDL_Point getMouseCell(int x, int y) const;
};
