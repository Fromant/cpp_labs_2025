#include "Game.hpp"

#include <iostream>

#include "Colors.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int GEM_WIDTH = 40;
constexpr int GEM_HEIGHT = 40;
constexpr int TOTAL_GEMS = SCREEN_WIDTH * SCREEN_HEIGHT / GEM_WIDTH / GEM_HEIGHT;

Game::Game() {
    field.reserve(TOTAL_GEMS);
}

bool Game::Initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Создание окна
    window = SDL_CreateWindow("Gems",
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Создание рендерера
    renderer = SDL_CreateRenderer(window,
                                  nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    InitField();

    return true;
}

void Game::InitField() {
    for (int i = 0; i < TOTAL_GEMS; i++) {
        Gem gem;

        gem.color = PALETTE[i % PALETTE_SIZE];

        field.emplace_back(gem);
    }
}


void Game::Run() {
    while (!shouldExit) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            shouldExit = true;
        }
        else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            HandleClick(e.button);
        }
    }
}

void Game::HandleClick(const SDL_MouseButtonEvent& e) {
    if (e.button != SDL_BUTTON_LEFT || !e.down) return;

    const float x = e.x;
    const float y = e.y;


}


void Game::Render() const {
    //count of gems in 1 row
    constexpr int LINE_LENGTH = SCREEN_WIDTH / GEM_WIDTH;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < field.size(); i++) {
        int gemX = i % LINE_LENGTH;
        int gemY = i / LINE_LENGTH;

        SDL_FRect gem{
                static_cast<float>(gemX * GEM_WIDTH), static_cast<float>(gemY * GEM_HEIGHT),
                GEM_WIDTH, GEM_HEIGHT
            };

        const SDL_Color& color = field[i].color;

        SDL_SetRenderDrawColor(renderer,
                               color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &gem);
    }

    SDL_RenderPresent(renderer);
}

void Game::CheckGems() {}

void Game::Update() {}

void Game::SpawnBonuses() {}
