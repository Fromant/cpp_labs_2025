#pragma once

#include <vector>

#include "Token.hpp"
#include "PluginManager.hpp"

std::vector<Token> shuntingYard(const std::vector<Token>& tokens, const IPluginRegistry& pm);