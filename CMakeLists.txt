cmake_minimum_required(VERSION 3.14)
project(EngineWrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(EngineWrapper main.cpp)
target_include_directories(EngineWrapper PRIVATE src)

add_subdirectory(tests)
