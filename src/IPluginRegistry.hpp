#pragma once

#include <string>
#include "plugin_interface.h"

class IPluginRegistry {
public:
    virtual ~IPluginRegistry() = default;

    struct TokenInfo {
        int arity;
        int precedence;
        Associativity associativity;
        bool is_operator;
        PluginResult (*evaluate)(const double*, size_t);
    };

    virtual bool hasToken(const std::string& name) const = 0;
    virtual const TokenInfo& getTokenInfo(const std::string& name) const = 0;
};
