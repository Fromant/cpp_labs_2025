#pragma once

#include <string>

#include "PluginManager.hpp"

class Calculator {
    PluginManager pm;
public:
    double evaluate(const std::string& expression);
};
