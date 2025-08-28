#include "bonuses/ShrinkPaddleBonus.hpp"

void ShrinkPaddleBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
}

void ShrinkPaddleBonus::activate(GameState& game) {
    activated = true;
    game.paddleWidthMultiplier /= SHRINK_PADDLE_MULTIPLIER;
    doRender=false;
    activated=true;
}

void ShrinkPaddleBonus::deactivate(GameState& game) {
    game.paddleWidthMultiplier *= SHRINK_PADDLE_MULTIPLIER;
    active = false;
    activated=false;
}
