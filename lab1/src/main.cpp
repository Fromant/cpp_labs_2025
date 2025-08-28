#include "Game.hpp"

int main() {
    Game gems{};

    if (gems.Initialize()) {
        gems.Run();
    }

    return 0;
}
