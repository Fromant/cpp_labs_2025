#include "bonuses/StickyPaddleBonus.hpp"

void StickyPaddleBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
}

void StickyPaddleBonus::activate(GameState& game) {
    game.stickyPaddle = true;
    activated = true;
    doRender = false;
}

void StickyPaddleBonus::deactivate(GameState& game) {
    game.stickyPaddle = false;
    active = false;
}

void StickyPaddleBonus::expire(GameState& state) {
    deactivate(state);
}

void StickyPaddleBonus::update(float deltaTime, GameState& state) {
    Bonus::update(deltaTime, state);
    if (activated) IExpirable::update(deltaTime, state);
}
