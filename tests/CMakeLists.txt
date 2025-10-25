# Download and build GoogleTest if not found
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
file(GLOB TEST_SOURCES "*.cpp")
add_executable(CalcTests ${TEST_SOURCES})
target_link_libraries(CalcTests CalcLib gtest gmock gtest_main)
target_include_directories(CalcTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_test(NAME CalcTests COMMAND CalcTests)