#include "bonuses/AccelerateBallBonus.hpp"

void AccelerateBallBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
    activated = true;
}

void AccelerateBallBonus::activate(GameState& game) {
    game.ballSpeedMultiplier *= ACCELERATE_BALL_MULTIPLIER;
    activated = true;
    doRender = false;
}

void AccelerateBallBonus::deactivate(GameState& game) {
    game.ballSpeedMultiplier /= ACCELERATE_BALL_MULTIPLIER;
    active = false;
    activated = false;
}
