#pragma once

constexpr int LIVES = 1;
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr float BASE_BALL_SPEED = 300.0f;
constexpr int BASE_PADDLE_WIDTH = 100;
constexpr int BASE_PADDLE_HEIGHT = 20;
constexpr float BASE_BALL_HEIGHT = 20;
constexpr float BASE_BALL_WIDTH = 20;

struct GameState {
    GameState() = default;

    //copy assignment
    GameState& operator=(GameState const& other) {
        score = other.score;
        lives = other.lives;
        stickyPaddle = other.stickyPaddle;
        safetyNetActive = other.safetyNetActive;
        ballSticked = other.ballSticked;
        paddleSpeedMultiplier = other.paddleSpeedMultiplier;
        ballSpeedMultiplier = other.ballSpeedMultiplier;
        paddleWidthMultiplier = other.paddleWidthMultiplier;
        ballVelocity = other.ballVelocity;
        return *this;
    }

    int score = 0;
    int lives = LIVES;
    bool stickyPaddle = false;
    bool safetyNetActive = true;
    bool ballSticked = true;
    float paddleSpeedMultiplier = 1.0f;
    float ballSpeedMultiplier = 1.0f;
    float paddleWidthMultiplier = 1.0f;
    SDL_FPoint ballVelocity{0, -BASE_BALL_SPEED};
    const float maxBallSpeed = 500.0f; // Максимальная скорость мяча
};
