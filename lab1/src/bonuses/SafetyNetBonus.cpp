#include "bonuses/SafetyNetBonus.hpp"

void SafetyNetBonus::render(SDL_Renderer* renderer) {
    if (!doRender) return;
    SDL_SetRenderDrawColor(renderer, COLOR.r, COLOR.g, COLOR.b, COLOR.a);
    SDL_RenderFillRect(renderer, &rect);
}

void SafetyNetBonus::activate(GameState& game) {
    game.safetyNetActive = true;
    activated = true;
    doRender = false;
}

void SafetyNetBonus::deactivate(GameState& game) {
    game.safetyNetActive = false;
    active = false;
    activated = false;
}
