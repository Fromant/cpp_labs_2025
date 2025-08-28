#include "Gem.hpp"
#include "Bonus.hpp"

Gem::Gem(SDL_Color c) : color(c) {}

void Gem::Draw(SDL_Renderer* renderer, const SDL_FRect& rect) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    if (bonus) {
        bonus->Draw(renderer, rect);
    }
}

void Gem::SetBonus(std::unique_ptr<Bonus> b) {
    bonus = std::move(b);
}

bool Gem::HasBonus() const {
    return bonus != nullptr;
}

void Gem::ActivateBonus(Game* game, size_t index) {
    if (bonus) {
        bonus->Activate(game, index);
        bonus.reset();
    }
}