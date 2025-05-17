cmake_minimum_required(VERSION 3.15)
project(gems)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "main"
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SDL3)

add_executable(gems
        src/main.cpp
        src/Game.cpp
)

target_include_directories(gems PRIVATE include)
target_link_libraries(gems PRIVATE SDL3::SDL3)