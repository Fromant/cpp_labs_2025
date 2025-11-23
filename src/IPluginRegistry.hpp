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
        bool is_operator; // now mostly for debug; type is known from context
        PluginResult (*evaluate)(const double*, size_t);
    };

    virtual bool hasFunction(const std::string& name) const = 0;
    virtual bool hasUnaryOperator(const std::string& name) const = 0;
    virtual bool hasBinaryOperator(const std::string& name) const = 0;

    virtual const TokenInfo& getFunction(const std::string& name) const = 0;
    virtual const TokenInfo& getUnaryOperator(const std::string& name) const = 0;
    virtual const TokenInfo& getBinaryOperator(const std::string& name) const = 0;
};
