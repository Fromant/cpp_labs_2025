#pragma once

#include <string>
#include <functional>

#include "plugin_interface.h"

class PluginManager {
public:
    struct TokenInfo {
        int arity;
        int precedence;
        Associativity associativity;
        bool is_operator;
        double (*evaluate)(const double*, size_t);
    };

    PluginManager();
    ~PluginManager();

    bool hasToken(const std::string& name) const;
    const TokenInfo& getTokenInfo(const std::string& name) const;

private:
    void loadPlugins();
    void loadPlugin(const std::string& path);
    void validatePlugin(FunctionInfo const* info);

    std::unordered_map<std::string, TokenInfo> registry_;

    std::vector<void*> handles_;
};
