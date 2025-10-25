# Download and build GoogleTest if not found
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
file(GLOB TEST_SOURCES "*.cpp")
add_executable(calculator_tests ${TEST_SOURCES})
target_link_libraries(calculator_tests calculator gtest gmock)
target_include_directories(calculator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_test(NAME calculator_tests COMMAND calculator_tests)