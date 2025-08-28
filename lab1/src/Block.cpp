#include "Block.hpp"

Block::Block(float x, float y, float w, float h, int hp, bool indestruct, bool accelerateBall, Bonus* bonus) :
    rect{x, y, w, h},
    health(hp),
    indestructible(indestruct),
    accelerateBall(accelerateBall),
    color{128, 128, 128, 255},
    innerBonus(bonus) {}

void Block::hit(GameState& gameState, SDL_FRect& collisionResult) {
    if (!indestructible) {
        health--;
        gameState.score++;
    }
    if (accelerateBall) {
        gameState.ballSpeedMultiplier *= ACCELERATION_BLOCK_MULTIPLIER;
    }
}

bool Block::IsDestroyed() const {
    return health <= 0 && !indestructible;
}
