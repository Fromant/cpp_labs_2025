#include "Game.hpp"
#include "Gem.hpp"
#include "Bonus.hpp"
#include <iostream>
#include <random>
#include <algorithm>

Game::Game() {
    field.reserve(TOTAL_GEMS);
}

bool Game::Initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Gems", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    InitField();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

int getRandomInt(int start, int end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(start, end - 1);
    return dist(gen);
}

bool operator==(const SDL_Color& a, const SDL_Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool Game::canPlace(size_t i, SDL_Color src) const {
    if (i >= 2 * LINE_LENGTH) {
        auto gem1 = std::dynamic_pointer_cast<Gem>(field[i - LINE_LENGTH]);
        auto gem2 = std::dynamic_pointer_cast<Gem>(field[i - 2 * LINE_LENGTH]);
        if (gem1 && gem2 && gem1->color == src && gem2->color == src)
            return false;
    }

    if (i % LINE_LENGTH >= 2) {
        auto gem1 = std::dynamic_pointer_cast<Gem>(field[i - 1]);
        auto gem2 = std::dynamic_pointer_cast<Gem>(field[i - 2]);
        if (gem1 && gem2 && gem1->color == src && gem2->color == src)
            return false;
    }
    return true;
}

void Game::InitField() {
    for (int i = 0; i < TOTAL_GEMS; i++) {
        auto color = PALETTE[getRandomInt(0, PALETTE_SIZE)];
        while (!canPlace(i, color)) {
            color = PALETTE[getRandomInt(0, PALETTE_SIZE)];
        }
        field.push_back(std::make_shared<Gem>(color));
    }
}

void Game::Run() {
    while (!shouldExit) {
        ProcessInput();
        Update();
        Render();
        SDL_Delay(100);
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

bool isNeighbours(size_t a, size_t b) {
    if (a == b) return false;
    if (a > b) std::swap(a, b);
    if (b - a == 1 && a % LINE_LENGTH != LINE_LENGTH-1) return true;
    if (b - a == LINE_LENGTH) return true;
    return false;
}

void Game::HandleClick(const SDL_MouseButtonEvent& e) {
    if (e.button != SDL_BUTTON_LEFT || !e.down) {
        selectedGem = -1;
        return;
    }

    const int x = e.x / GEM_WIDTH;
    const int y = e.y / GEM_HEIGHT;

    size_t clicked = x + y * LINE_LENGTH;
    if (selectedGem != -1 && isNeighbours(selectedGem, clicked)) {
        std::swap(field[selectedGem], field[clicked]);
        selectedGem = -1;
    }
    else {
        selectedGem = clicked;
    }
}

void Game::Render() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < field.size(); i++) {
        int gemX = i % LINE_LENGTH;
        int gemY = i / LINE_LENGTH;

        SDL_FRect gemRect{
            static_cast<float>(gemX * GEM_WIDTH),
            static_cast<float>(gemY * GEM_HEIGHT),
            static_cast<float>(GEM_WIDTH),
            static_cast<float>(GEM_HEIGHT)
        };

        field[i]->Draw(renderer, gemRect);
    }

    if (selectedGem != -1 && selectedGem < field.size()) {
        int gemX = selectedGem % LINE_LENGTH;
        int gemY = selectedGem / LINE_LENGTH;
        SDL_FRect gemRect{
            static_cast<float>(gemX * GEM_WIDTH),
            static_cast<float>(gemY * GEM_HEIGHT),
            static_cast<float>(GEM_WIDTH),
            static_cast<float>(GEM_HEIGHT)
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 96);
        SDL_RenderFillRect(renderer, &gemRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &gemRect);
    }

    SDL_RenderPresent(renderer);
}

bool Game::CheckTriplets() {
    bool located = false;
    for (int i = 2; i < TOTAL_GEMS; i++) {
        auto gem_i = std::dynamic_pointer_cast<Gem>(field[i]);
        if (!gem_i) continue;

        const auto& src = gem_i->color;

        // Проверка вертикальных троек
        if (i >= 2 * LINE_LENGTH) {
            auto gem_up1 = std::dynamic_pointer_cast<Gem>(field[i - LINE_LENGTH]);
            auto gem_up2 = std::dynamic_pointer_cast<Gem>(field[i - 2 * LINE_LENGTH]);
            if (gem_up1 && gem_up2 && gem_up1->color == src && gem_up2->color == src) {
                located = true;
                int additional = 1;
                while (i + additional * LINE_LENGTH < TOTAL_GEMS) {
                    auto gem_next = std::dynamic_pointer_cast<Gem>(field[i + additional * LINE_LENGTH]);
                    if (gem_next && gem_next->color == src)
                        additional++;
                    else
                        break;
                }
                additional--;

                // Определяем столбец
                int col = i % LINE_LENGTH;
                int startRow = i / LINE_LENGTH - 2; // начальная строка тройки
                int endRow = startRow + 2 + additional; // последняя строка в цепочке

                // Спавним бонусы для всех гемов в цепочке
                for (int row = startRow; row <= endRow; row++) {
                    int idx = col + row * LINE_LENGTH;
                    SpawnBonus(idx);
                }

                // Количество удаляемых строк
                int rowsToRemove = 3 + additional;

                // Сдвигаем гемы в столбце сверху вниз
                for (int row = endRow; row >= rowsToRemove; row--) {
                    int idx = col + row * LINE_LENGTH;
                    field[idx] = field[idx - rowsToRemove * LINE_LENGTH];
                }

                // Генерируем новые гемы наверху
                for (int row = 0; row < rowsToRemove; row++) {
                    int idx = col + row * LINE_LENGTH;
                    field[idx] = std::make_shared<Gem>(PALETTE[getRandomInt(0, PALETTE_SIZE)]);
                }

                break;
            }
        }

        // Проверка горизонтальных троек
        if (i % LINE_LENGTH >= 2) {
            auto gem_left1 = std::dynamic_pointer_cast<Gem>(field[i - 1]);
            auto gem_left2 = std::dynamic_pointer_cast<Gem>(field[i - 2]);
            if (gem_left1 && gem_left2 && gem_left1->color == src && gem_left2->color == src) {
                located = true;
                int additional = 1;
                int row = i / LINE_LENGTH;
                int endLineIndex = (row + 1) * LINE_LENGTH;
                while (i + additional < endLineIndex) {
                    auto gem_next = std::dynamic_pointer_cast<Gem>(field[i + additional]);
                    if (gem_next && gem_next->color == src)
                        additional++;
                    else
                        break;
                }
                additional--;

                int startCol = i % LINE_LENGTH - 2;
                int endCol = startCol + 2 + additional;

                // Спавним бонусы для всех гемов в цепочке
                for (int col = startCol; col <= endCol; col++) {
                    int idx = col + row * LINE_LENGTH;
                    SpawnBonus(idx);
                }

                // Для каждого столбца в цепочке сдвигаем гемы сверху вниз
                for (int col = startCol; col <= endCol; col++) {
                    int idx = col + row * LINE_LENGTH;

                    // Сдвигаем все гемы выше вниз на одну позицию
                    for (int r = row; r > 0; r--) {
                        int currentIdx = col + r * LINE_LENGTH;
                        int aboveIdx = col + (r-1) * LINE_LENGTH;
                        field[currentIdx] = field[aboveIdx];
                    }

                    // Генерируем новый гем наверху
                    field[col] = std::make_shared<Gem>(PALETTE[getRandomInt(0, PALETTE_SIZE)]);
                }

                break;
            }
        }
    }
    return located;
}

void Game::Update() {
    ActivateBonuses();
    CheckTriplets();
}

void Game::ActivateBonuses() {
    for (size_t i = 0; i < field.size(); i++) {
        if (auto gem = std::dynamic_pointer_cast<Gem>(field[i])) {
            if (gem->HasBonus()) {
                gem->ActivateBonus(this, i);
            }
        }
    }
}

void Game::SpawnBonus(size_t destroyed) {
    constexpr float SPAWN_CHANCE = 0.5;
    if (static_cast<float>(getRandomInt(0, 101)) / 100 > SPAWN_CHANCE) return;

    constexpr int SPAWN_RADIUS = 3;
    int x = destroyed % LINE_LENGTH;
    int y = destroyed / LINE_LENGTH;

    int xNew = getRandomInt(std::max(0, x - SPAWN_RADIUS), std::min(LINE_LENGTH, x + SPAWN_RADIUS));
    int yNew = getRandomInt(std::max(0, y - SPAWN_RADIUS), std::min(ROWS, y + SPAWN_RADIUS));
    size_t idx = xNew + yNew * LINE_LENGTH;

    if (idx >= field.size()) return;
    if (auto gem = std::dynamic_pointer_cast<Gem>(field[idx])) {
        std::unique_ptr<Bonus> bonus;
        if (getRandomInt(0, 2) == 0) {
            bonus = std::make_unique<RecolorBonus>();
        } else {
            bonus = std::make_unique<BombBonus>();
        }
        gem->SetBonus(std::move(bonus));
    }
}