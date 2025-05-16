#include "Bonus.hpp"
#include "Game.hpp"

Bonus::Bonus(float x, float y, BonusType t) :
    rect{x, y, 30, 15}, type(t), active(true), fallSpeed(0.2f) {}

void Bonus::Update(float deltaTime) {
    rect.y += fallSpeed * deltaTime;
}

void Bonus::Activate(Game& game) {
    game.ActivateBonus(type);
}