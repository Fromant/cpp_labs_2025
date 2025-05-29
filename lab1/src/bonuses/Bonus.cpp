#include "bonuses/Bonus.hpp"

Bonus::Bonus(float x, float y) : rect{x, y, BONUS_WIDTH, BONUS_HEIGHT} {}

void Bonus::update(float deltaTime, GameState& state) {\
    if (!doRender) return;
    rect.y += fallSpeed * deltaTime;
    if (rect.y > SCREEN_HEIGHT) {
        active = false;
    }
}
