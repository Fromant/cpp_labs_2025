#pragma once
#include "SDL3/SDL.h"
#include <vector>
#include <memory>
#include "Colors.hpp"
#include "GameObject.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int GEM_WIDTH = 40;
constexpr int GEM_HEIGHT = 40;
constexpr int TOTAL_GEMS = SCREEN_WIDTH * SCREEN_HEIGHT / GEM_WIDTH / GEM_HEIGHT;
constexpr int LINE_LENGTH = SCREEN_WIDTH / GEM_WIDTH;
constexpr int ROWS = SCREEN_HEIGHT / GEM_HEIGHT;

class Game {
public:
    Game();
    bool Initialize();
    void Run();
    void ProcessInput();
    void Update();
    void Render() const;

    void HandleClick(const SDL_MouseButtonEvent& e);
    bool CheckTriplets();
    void ActivateBonuses();
    void SpawnBonus(size_t destroyed);

    // Для доступа бонусов
    std::vector<std::shared_ptr<GameObject>>& GetField() { return field; }
    friend class BombBonus;
    friend class RecolorBonus;

private:
    bool shouldExit = false;
    int selectedGem = -1;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::vector<std::shared_ptr<GameObject>> field;

    void InitField();
    bool canPlace(size_t i, SDL_Color src) const;
};