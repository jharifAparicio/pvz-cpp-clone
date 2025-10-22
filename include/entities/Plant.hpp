#pragma once
#include "entities/Entity.hpp"

class Plant : public Entity {
public:
  Plant(int x, int y, int w, int h);
  void update(float dt) override;
  void render(SDL_Renderer *renderer) override;

  bool canShoot() const { return shootTimer_ >= shootCooldown_; }
  void resetShootTimer() { shootTimer_ = 0.0; }

  void takeDamage(int dmg);
  bool isDead() const { return hp_ <= 0; }

private:
  double shootCooldown_ = 1.5; // segundos entre disparos
  double shootTimer_ = 0.0;

  double hp_ = 5.0;
};