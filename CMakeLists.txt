cmake_minimum_required(VERSION 3.14)
project(CalcApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_executable(calc
        src/Tokenizer.cpp
        src/PluginManager.cpp
        src/Token.hpp
        src/Tokenizer.hpp
        src/ShuntingYard.hpp
        src/ShuntingYard.cpp
        src/RpnEvaluator.hpp
        src/RpnEvaluator.cpp
        src/Calculator.hpp
        src/Calculator.cpp
        src/main.cpp
)

target_include_directories(calc PRIVATE src)

# Build plugins
add_subdirectory(plugins)

# Post-build: copy DLLs to ./plugins next to calc.exe
add_custom_command(TARGET calc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:calc>/plugins
)

foreach(plugin ${PLUGIN_TARGETS})
    add_custom_command(TARGET calc POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${plugin}>
            $<TARGET_FILE_DIR:calc>/plugins/
    )
endforeach()