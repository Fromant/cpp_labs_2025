cmake_minimum_required(VERSION 3.15)
project(arkanoid)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "main"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL3)

add_executable(arkanoid
        src/main.cpp
        src/Game.cpp
        src/Block.cpp
        src/bonuses/Bonus.cpp
        src/bonuses/AccelerateBallBonus.cpp
        src/bonuses/ExpandPaddleBonus.cpp
        src/bonuses/RandomBallDirectionBonus.cpp
        src/bonuses/SafetyNetBonus.cpp
        src/bonuses/ShrinkPaddleBonus.cpp
        src/bonuses/StickyPaddleBonus.cpp
)

target_include_directories(arkanoid PRIVATE include)
target_link_libraries(arkanoid PRIVATE SDL3::SDL3)