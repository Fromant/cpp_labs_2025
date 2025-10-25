#pragma once

#include <vector>

#include "Token.hpp"
#include "PluginManager.hpp"

double evaluateRpn(const std::vector<Token>& rpn, const PluginManager& pm);