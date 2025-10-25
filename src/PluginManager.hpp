#pragma once

#include <string>
#include <functional>

#include "IPluginRegistry.hpp"
#include "plugin_interface.h"

class PluginManager : public IPluginRegistry{
public:
    using IPluginRegistry::TokenInfo;  // reuse nested type

    PluginManager();
    ~PluginManager() override;

    bool hasToken(const std::string& name) const final;
    const TokenInfo& getTokenInfo(const std::string& name) const final;

private:
    void loadPlugins();
    void loadPlugin(const std::string& path);
    static void validatePlugin(FunctionInfo const* info);

    std::unordered_map<std::string, TokenInfo> registry_;

    std::vector<void*> handles_;
};
