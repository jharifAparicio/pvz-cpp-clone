#include "core/Game.hpp"
#include "core/Grid.hpp"
#include "entities/Plant.hpp"
#include "entities/Projectile.hpp"
#include "entities/Zombie.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

Game::~Game() { shutdown(); }

bool Game::init(const std::string &title, int width, int height, bool vsync) {
  width_ = width;
  height_ = height;
  vsync_ = vsync;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
    return false;
  }

  window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_,
                             height_, SDL_WINDOW_SHOWN);
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

  grid_ = std::make_unique<Grid>(5, 9, 120, 120, 100, 60);

  // Pruebas
  entities_.push_back(std::make_unique<Plant>(150, 80, 60, 80));
  entities_.push_back(std::make_unique<Zombie>(1000, 80, 60, 80, 60.0f));
  entities_.push_back(std::make_unique<Projectile>(210, 110, 20, 20, 300.0f));

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

void Game::processEvents(bool &running) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      running = false;
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
      running = false;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
      int mx = e.button.x;
      int my = e.button.y;
      SDL_Point cell = getMouseCell(mx, my);

      if (cell.x >= 0 && cell.x < grid_->getCols() && cell.y >= 0 && cell.y < grid_->getRows()) {
        if (isCellEmpty(cell.y, cell.x)) {
          SDL_Rect r = grid_->getCell(cell.y, cell.x);
          entities_.push_back(std::make_unique<Plant>(r.x + 30, r.y + 20, 60, 80));
        }
      }
    }
  }
}

void Game::update(float dt) {
  // --- 1. Actualizar entidades existentes ---
  for (auto &e : entities_) {
    e->update(dt);
  }

  // --- 2. Spawner de zombies ---
  std::vector<std::unique_ptr<Entity>> newEntities;

  zombieTimer_ += dt;
  if (zombieTimer_ >= zombieCooldown_) {
    zombieTimer_ = 0.0;

    int row = rand() % grid_->getRows();
    SDL_Rect cell = grid_->getCell(row, grid_->getCols() - 1);

    newEntities.push_back(std::make_unique<Zombie>(cell.x + 20, cell.y + 20, 60, 80, 100.0f, 10));
  }

  // --- 3. Disparo de plantas (solo si hay zombies en su fila) ---
  for (auto &e : entities_) {
    auto p = dynamic_cast<Plant *>(e.get());
    if (!p)
      continue;

    SDL_Rect pr = p->getRect();
    bool zombieInRow = false;

    for (auto &z : entities_) {
      auto zombie = dynamic_cast<Zombie *>(z.get());
      if (zombie) {
        SDL_Rect zr = zombie->getRect();
        if (abs(zr.y - pr.y) < 40 && zr.x > pr.x) {
          zombieInRow = true;
          break;
        }
      }
    }

    if (zombieInRow && p->canShoot()) {
      newEntities.push_back(
          std::make_unique<Projectile>(pr.x + pr.w, pr.y + pr.h / 2 - 5, 20, 10, 350.0f));
      p->resetShootTimer();
    }
  }

  // --- 4. Colisiones proyectil-zombie ---
  for (auto &e : entities_) {
    auto proj = dynamic_cast<Projectile *>(e.get());
    if (!proj)
      continue;

    for (auto &z : entities_) {
      auto zombie = dynamic_cast<Zombie *>(z.get());
      if (zombie && checkCollision(proj->getRect(), zombie->getRect())) {
        zombie->takeDamage(1);
        proj->update(9999.0f); // lo manda fuera de pantalla
      }
    }
  }

  // 🧟 Ataque de zombies a plantas
  for (auto &z : entities_) {
    auto zombie = dynamic_cast<Zombie *>(z.get());
    if (!zombie)
      continue;

    bool biting = false;

    for (auto &p : entities_) {
      auto plant = dynamic_cast<Plant *>(p.get());
      if (!plant)
        continue;

      SDL_Rect zr = zombie->getRect();
      SDL_Rect pr = plant->getRect();

      if (checkCollision(zr, pr)) {
        biting = true;
        zombie->setAttacking(true);

        if (zombie->isAttacking()) {
          // Daño cada 1 segundo
          if (zombie->isDead())
            break;              // no atacar si está muerto
          zombie->update(0.0f); // mantiene el timer
          if (zombie->isAttacking() && zombie->isDead())
            break;
        }
      }
    }

    if (!biting)
      zombie->setAttacking(false);
  }

  // Aplicar daño periódico
  for (auto &z : entities_) {
    auto zombie = dynamic_cast<Zombie *>(z.get());
    if (!zombie || !zombie->isAttacking())
      continue;

    for (auto &p : entities_) {
      auto plant = dynamic_cast<Plant *>(p.get());
      if (!plant)
        continue;

      SDL_Rect zr = zombie->getRect();
      SDL_Rect pr = plant->getRect();

      if (checkCollision(zr, pr)) {
        zombie->update(0.016f); // avanza timer ataque
        if (zombie->isDead())
          continue;

        if (zombie->isAttacking() && zombie->isDead())
          continue;

        if (zombie->isAttacking()) {
          // muerde cada segundo
          if (zombie->isDead())
            continue;
          plant->takeDamage(1);
        }
      }
    }
  }

  // --- 5. Eliminar entidades muertas o fuera de pantalla ---
  entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                 [this](const std::unique_ptr<Entity> &e) {
                                   auto r = e->getRect();
                                   if (auto proj = dynamic_cast<Projectile *>(e.get()))
                                     return r.x > width_;
                                   if (auto zombie = dynamic_cast<Zombie *>(e.get()))
                                     return zombie->isDead() || r.x + r.w < 0;
                                   if (auto plant = dynamic_cast<Plant *>(e.get()))
                                     return plant->isDead(); // plantas destruidas por mordida
                                   return false;
                                 }),
                  entities_.end());

  // --- 6. Agregar nuevas entidades seguras ---
  for (auto &n : newEntities) {
    entities_.push_back(std::move(n));
  }
}

void Game::render() {
  SDL_SetRenderDrawColor(renderer_, 90, 170, 90, 255);
  SDL_RenderClear(renderer_);

  grid_->render(renderer_);

  for (auto &e : entities_)
    e->render(renderer_);

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
  if (renderer_) {
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
  }
  if (window_) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
}

bool Game::isCellEmpty(int row, int col) const {
  SDL_Rect target = grid_->getCell(row, col);
  for (auto &e : entities_) {
    auto p = dynamic_cast<Plant *>(e.get());
    if (p) {
      SDL_Rect plantRect = p->getRect();
      if (SDL_HasIntersection(&target, &plantRect)) {
        return false;
      }
    }
  }
  return true;
}

SDL_Point Game::getMouseCell(int mouseX, int mouseY) const {
  int col = (mouseX - 100) / 120;
  int row = (mouseY - 60) / 120;
  return SDL_Point{col, row};
}