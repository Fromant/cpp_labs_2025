cmake_minimum_required(VERSION 3.14)
project(CalcApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_executable(CalcApp main.cpp)

target_link_libraries(CalcApp CalcLib)

# Build plugins
add_subdirectory(plugins)

# Post-build: copy DLLs to ./plugins next to calc.exe
add_custom_command(TARGET CalcApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:CalcApp>/plugins
)

foreach (plugin ${PLUGIN_TARGETS})
    add_custom_command(TARGET CalcApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${plugin}>
            $<TARGET_FILE_DIR:CalcApp>/plugins/
    )
endforeach ()

enable_testing()
add_subdirectory(tests)
