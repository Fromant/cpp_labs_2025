cmake_minimum_required(VERSION 3.14)
project(CalcLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CalcLib
        Tokenizer.cpp
        PluginManager.cpp
        Token.hpp
        Tokenizer.hpp
        ShuntingYard.hpp
        ShuntingYard.cpp
        RpnEvaluator.hpp
        RpnEvaluator.cpp
        Calculator.hpp
        Calculator.cpp
)
