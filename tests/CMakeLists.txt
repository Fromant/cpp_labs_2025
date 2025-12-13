cmake_minimum_required(VERSION 3.14)
project(EngineWrapperTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES "*.cpp")
add_executable(EngineWrapperTests ${TEST_SOURCES})
target_link_libraries(EngineWrapperTests gtest gtest_main)
target_include_directories(EngineWrapperTests PRIVATE ../src)

