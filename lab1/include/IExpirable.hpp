#pragma once
class IExpirable {
public:
    virtual ~IExpirable() = default;

    bool activated = false;

    IExpirable(float duration) : duration(duration) {}

    virtual void update(const float dt, GameState& state) {
        duration -= dt;
        if (duration < 0) expire(state);
    }

protected:
    float duration; // in seconds

    virtual void expire(GameState& state) = 0;
};
