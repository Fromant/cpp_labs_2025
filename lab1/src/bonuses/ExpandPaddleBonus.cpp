#include "bonuses/ExpandPaddleBonus.hpp"

void ExpandPaddleBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
}

void ExpandPaddleBonus::activate(GameState& game) {
    game.paddleWidthMultiplier *= EXPAND_PADDLE_MULTIPLIER;
    activated = true;
    doRender = false;
}

void ExpandPaddleBonus::deactivate(GameState& game) {
    game.paddleWidthMultiplier /= EXPAND_PADDLE_MULTIPLIER;
    active = false;
    activated = false;
}
