#include "Game.hpp"
#include <random>
#include <algorithm>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr float BASE_PADDLE_SPEED = 200.0f;
constexpr int BASE_PADDLE_WIDTH = 100;
constexpr int BASE_PADDLE_HEIGHT = 20;
constexpr float BASE_BALL_SPEED = 100.0f;
constexpr int LIVES = 1;
constexpr Uint64 STICKY_PADDLE_DURATION = 10000; //10s
constexpr Uint64 SAFETY_NET_DURATION = 10000; //10s

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true), needsReset(false),
               score(0), lives(LIVES), stickyPaddle(false), safetyNetActive(false),
               paddleSpeedMultiplier(1.0f), ballSpeedMultiplier(1.0f) {}

void Game::ResetGame() {
    // Reset game state
    lives = LIVES;
    score = 0;
    stickyPaddle = false;
    safetyNetActive = false;
    paddleSpeedMultiplier = 1.0f;
    ballSpeedMultiplier = 1.0f;

    // Reset paddle and ball
    paddle = {
            SCREEN_WIDTH / 2 - BASE_PADDLE_WIDTH / 2, SCREEN_HEIGHT - 2 * BASE_PADDLE_HEIGHT, BASE_PADDLE_WIDTH,
            BASE_PADDLE_HEIGHT
        };
    ball = {SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10, 20, 20};
    ballVelocity = {BASE_BALL_SPEED, -BASE_BALL_SPEED};

    // Rebuild blocks
    blocks.clear();
    const int rows = 4;
    const int cols = 10;
    const float blockWidth = 70;
    const float blockHeight = 20;
    const float spacing = 5;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> bonusDist(1, 6);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float x = j * (blockWidth + spacing) + spacing;
            float y = i * (blockHeight + spacing) + 50;
            BonusType bonus = BonusType::None;
            bool indestruct = (i == 0);
            int health = indestruct ? 1 : (rows - i);

            if (!indestruct && (gen() % 4) == 0) {
                bonus = static_cast<BonusType>(bonusDist(gen));
            }

            blocks.emplace_back(std::make_unique<Block>(
                x, y, blockWidth, blockHeight,
                health, indestruct, bonus
            ));
        }
    }

    activeBonuses.clear();
}

void Game::CheckCollisions() {
    // Обработка границ экрана
    if (ball.x < 0) {
        ball.x = 0;
        ballVelocity.x = fabs(ballVelocity.x);
    }
    if (ball.x + ball.w > SCREEN_WIDTH) {
        ball.x = SCREEN_WIDTH - ball.w;
        ballVelocity.x = -fabs(ballVelocity.x);
    }
    if (ball.y < 0) {
        ball.y = 0;
        ballVelocity.y = fabs(ballVelocity.y);
    }

    // Коллизия с кареткой
    SDL_FRect intersection;
    if (SDL_GetRectIntersectionFloat(&ball, &paddle, &intersection)) {
        // Корректировка позиции мяча
        ball.y = paddle.y - ball.h;

        // Расчет точки удара [-1, 1]
        float hitPosition = (ball.x + ball.w / 2 - paddle.x) / paddle.w * 2 - 1;
        ballVelocity.x = hitPosition * fabs(ballVelocity.x + ballVelocity.y);
        ballVelocity.y = -fabs(ballVelocity.y);
    }

    // Коллизия с блоками
    for (auto it = blocks.begin(); it != blocks.end();) {
        SDL_FRect blockRect = (*it)->rect;
        SDL_FRect intersect;

        if (SDL_GetRectIntersectionFloat(&ball, &blockRect, &intersect)) {
            // Определение направления отскока
            float overlapLeft = ball.x + ball.w - blockRect.x;
            float overlapRight = blockRect.x + blockRect.w - ball.x;
            float overlapTop = ball.y + ball.h - blockRect.y;
            float overlapBottom = blockRect.y + blockRect.h - ball.y;

            bool horizontalCollision = std::min(overlapLeft, overlapRight) <
                std::min(overlapTop, overlapBottom);

            if (horizontalCollision) {
                ballVelocity.x = (overlapLeft < overlapRight) ? -fabs(ballVelocity.x) : fabs(ballVelocity.x);
                ball.x += (overlapLeft < overlapRight) ? -intersect.w : intersect.w;
            }
            else {
                ballVelocity.y = (overlapTop < overlapBottom) ? -fabs(ballVelocity.y) : fabs(ballVelocity.y);
                ball.y += (overlapTop < overlapBottom) ? -intersect.h : intersect.h;
            }

            // Обработка удара по блоку
            if (!(*it)->indestructible) {
                (*it)->health--;
                score++;

                if ((*it)->health <= 0) {
                    if ((*it)->hasBonus) {
                        SpawnBonus(blockRect.x + blockRect.w / 2,
                                   blockRect.y + blockRect.h / 2,
                                   (*it)->bonus);
                    }
                    it = blocks.erase(it);
                    continue;
                }
            }
        }
        ++it;
    }

    // Коллизия бонусов с кареткой
    for (auto it = activeBonuses.begin(); it != activeBonuses.end();) {
        SDL_FRect bonusRect = (*it)->rect;
        if (SDL_GetRectIntersectionFloat(&bonusRect, &paddle, &intersection)) {
            ActivateBonus((*it)->type);
            it = activeBonuses.erase(it);
        }
        else {
            if (bonusRect.y > SCREEN_HEIGHT) it = activeBonuses.erase(it);
            else ++it;
        }
    }
}

void Game::HandleBlockHit(Block& block) {
    if (!block.indestructible) {
        block.Hit();
        score += 1; // +1 очко за попадание
    }

    // Изменение скорости мяча для специальных блоков
    if (block.health == 3) {
        // Пример для блока, увеличивающего скорость
        ballSpeedMultiplier *= 1.2f;
    }
}


void Game::HandleBallBlockCollision(Block& block) {
    block.Hit();

    // Calculate collision normal
    float ballCenterX = ball.x + ball.w / 2;
    float ballCenterY = ball.y + ball.h / 2;
    float blockCenterX = block.rect.x + block.rect.w / 2;
    float blockCenterY = block.rect.y + block.rect.h / 2;

    float dx = ballCenterX - blockCenterX;
    float dy = ballCenterY - blockCenterY;
    float absDx = fabs(dx);
    float absDy = fabs(dy);

    if (absDx > absDy) {
        // Horizontal collision
        ballVelocity.x *= -1;
    }
    else {
        // Vertical collision
        ballVelocity.y *= -1;
    }
}

void Game::ActivateBonus(BonusType type) {
    switch (type) {
        case BonusType::ExpandPaddle:
            paddle.w = BASE_PADDLE_WIDTH * 1.5f;
            break;

        case BonusType::ShrinkPaddle:
            paddle.w = BASE_PADDLE_WIDTH * 0.75f;
            break;

        case BonusType::SpeedUp:
            ballSpeedMultiplier *= 1.2f;
            break;

        case BonusType::StickyPaddle:
            stickyPaddle = true;
            stickyPaddleExpireTime = SDL_GetTicks() + STICKY_PADDLE_DURATION;
            break;

        case BonusType::SafetyNet:
            safetyNetActive = true;
            safetyNetExpireTime = SDL_GetTicks() + SAFETY_NET_DURATION;
            break;

        case BonusType::RandomDirection: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f);
            ballVelocity.x += angleDist(gen);
            ballVelocity.y += angleDist(gen);
            // Normalize speed
            float speed = sqrtf(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
            ballVelocity.x = (ballVelocity.x / speed) * BASE_BALL_SPEED * ballSpeedMultiplier;
            ballVelocity.y = (ballVelocity.y / speed) * BASE_BALL_SPEED * ballSpeedMultiplier;
            break;
        }

        default:
            break;
    }
}

void Game::SpawnBonus(float x, float y, BonusType type) {
    auto bonus = std::make_unique<Bonus>(x, y, type);
    activeBonuses.push_back(std::move(bonus));
}


void Game::Run() {
    Uint64 previousTicks = SDL_GetTicks();

    while (isRunning) {
        Uint64 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Game::UpdatePaddle(const float deltaTime) {
    // Клавиатурный ввод для движения каретки
    const bool* keyboardState = SDL_GetKeyboardState(nullptr);
    float moveSpeed = BASE_PADDLE_SPEED * paddleSpeedMultiplier * deltaTime;

    if (keyboardState[SDL_SCANCODE_LEFT]) {
        paddle.x -= moveSpeed;
    }
    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        paddle.x += moveSpeed;
    }

    // Ограничение движения каретки
    if (paddle.x < 0) paddle.x = 0;
    if (paddle.x > SCREEN_WIDTH - paddle.w) {
        paddle.x = SCREEN_WIDTH - paddle.w;
    }
}

void Game::UpdateBonuses(const float deltaTime) {
    // Движение бонусов вниз
    for (auto& bonus : activeBonuses) {
        bonus->rect.y += 150.0f * deltaTime; // Скорость падения
        if (bonus->rect.y > SCREEN_HEIGHT) {
            bonus->active = false;
        }
    }

    // Удаление неактивных бонусов
    activeBonuses.erase(
        std::remove_if(activeBonuses.begin(), activeBonuses.end(),
                       [](const auto& b) { return !b->active; }),
        activeBonuses.end()
    );

    // Обновление бонусов
    for (auto& bonus : activeBonuses) {
        bonus->Update(deltaTime);
    }

    // Проверка времени действия бонусов
    Uint64 currentTime = SDL_GetTicks();
    if (stickyPaddle && currentTime > safetyNetExpireTime) {
        stickyPaddle = false;
    }
    if (safetyNetActive && currentTime > safetyNetExpireTime) {
        safetyNetActive = false;
    }
}

void Game::Update(const float deltaTime) {
    // Обновление позиции мяча с учётом множителя скорости
    ball.x += ballVelocity.x * deltaTime * ballSpeedMultiplier;
    ball.y += ballVelocity.y * deltaTime * ballSpeedMultiplier;

    // Проверка поражения
    if (ball.y + ball.h > SCREEN_HEIGHT) {
        if (safetyNetActive) {
            // Отскок от защитной сетки
            ballVelocity.y = -fabs(ballVelocity.y);
            safetyNetActive = false;
        }
        else {
            lives--;
            if (lives > 0) {
                // Респавн мяча
                ball = {SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10, 20, 20};
                ballVelocity = {BASE_BALL_SPEED, -BASE_BALL_SPEED};
            }
            else {
                ResetGame();
            }
        }
    }

    CheckCollisions();
    UpdateBonuses(deltaTime);
    UpdatePaddle(deltaTime);

    // Прилипание мяча к каретке
    if (stickyPaddle) {
        ball.x = paddle.x + paddle.w / 2 - ball.w / 2;
        ball.y = paddle.y - ball.h;
    }

    // Удаление разрушенных блоков
    blocks.erase(
        std::remove_if(blocks.begin(), blocks.end(),
                       [](const auto& block) { return block->IsDestroyed(); }),
        blocks.end()
    );
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Отрисовка каретки
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle);

    // Отрисовка мяча
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ball);

    // Отрисовка блоков
    for (const auto& block : blocks) {
        SDL_SetRenderDrawColor(renderer,
                               block->color.r,
                               block->color.g,
                               block->color.b,
                               block->color.a
        );
        SDL_RenderFillRect(renderer, &block->rect);

        // Для неразрушаемых блоков добавляем рамку
        if (block->indestructible) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &block->rect);
        }
    }

    // Отрисовка бонусов
    for (const auto& bonus : activeBonuses) {
        SDL_Color bonusColor;
        switch (bonus->type) {
            case BonusType::ExpandPaddle: bonusColor = {0, 255, 0, 255};
                break;
            case BonusType::ShrinkPaddle: bonusColor = {255, 0, 0, 255};
                break;
            case BonusType::SpeedUp: bonusColor = {255, 255, 0, 255};
                break;
            case BonusType::StickyPaddle: bonusColor = {0, 255, 255, 255};
                break;
            case BonusType::SafetyNet: bonusColor = {255, 165, 0, 255};
                break;
            case BonusType::RandomDirection: bonusColor = {255, 0, 255, 255};
                break;
            default: bonusColor = {255, 255, 255, 255};
        }
        SDL_SetRenderDrawColor(renderer,
                               bonusColor.r, bonusColor.g, bonusColor.b, bonusColor.a);
        SDL_RenderFillRect(renderer, &bonus->rect);
    }

    // Отрисовка HUD
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_FRect bottomLine = {0, SCREEN_HEIGHT - 2, SCREEN_WIDTH, 2};
    if (safetyNetActive) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderLine(renderer, 0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, SCREEN_HEIGHT - 20);
    }

    SDL_RenderPresent(renderer);
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                // Рестарт игры по R
                if (event.key.key == SDLK_R) {
                    ResetGame();
                }
                // Отпускание мяча при липкой каретке
                if (stickyPaddle && event.key.key == SDLK_SPACE) {
                    stickyPaddle = false;
                    ballVelocity = {
                            BASE_BALL_SPEED * ballSpeedMultiplier,
                            -BASE_BALL_SPEED * ballSpeedMultiplier
                        };
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                // Отпускание мяча по клику мыши
                if (stickyPaddle) {
                    stickyPaddle = false;
                    ballVelocity = {
                            BASE_BALL_SPEED * ballSpeedMultiplier,
                            -BASE_BALL_SPEED * ballSpeedMultiplier
                        };
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                // Управление кареткой мышью
                if (!stickyPaddle) {
                    paddle.x = event.motion.x - paddle.w / 2;
                }
                break;
            default: continue;
        }
    }
}

bool Game::Initialize() {
    // Инициализация SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Создание окна
    window = SDL_CreateWindow("Arkanoid",
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

    // Инициализация игрового состояния
    paddleSpeedMultiplier = 1.0f;
    ballSpeedMultiplier = 1.0f;
    safetyNetExpireTime = 0;
    stickyPaddle = false;
    safetyNetActive = false;

    // Настройка рандомизации
    std::srand(static_cast<unsigned>(SDL_GetTicks()));

    // Инициализация игровых объектов
    ResetGame();

    // Начальные координаты каретки
    paddle = {
            SCREEN_WIDTH / 2 - BASE_PADDLE_WIDTH / 2,
            SCREEN_HEIGHT - 2 * BASE_PADDLE_HEIGHT,
            BASE_PADDLE_WIDTH,
            BASE_PADDLE_HEIGHT
        };

    // Начальное состояние мяча
    ball = {SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 10, 20, 20};
    ballVelocity = {BASE_BALL_SPEED, -BASE_BALL_SPEED};

    return true;
}

Game::~Game() = default;
