#include "Block.hpp"

Block::Block(float x, float y, float w, float h, int hp, bool indestruct, BonusType bonusType) :
    rect{x, y, w, h},
    health(hp),
    indestructible(indestruct),
    bonus(bonusType),
    hasBonus(bonusType != BonusType::None),
    color{128, 128, 128, 255} {}

void Block::Hit() {
    if(!indestructible) health--;
}

bool Block::IsDestroyed() const {
    return health <= 0 && !indestructible;
}