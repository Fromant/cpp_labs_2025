#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "IPluginRegistry.hpp"
#include "plugin_interface.h"

class PluginManager : public IPluginRegistry {
public:
    PluginManager();
    ~PluginManager() override;

    bool hasFunction(const std::string& name) const final;
    bool hasUnaryOperator(const std::string& name) const final;
    bool hasBinaryOperator(const std::string& name) const final;

    const TokenInfo& getFunction(const std::string& name) const final;
    const TokenInfo& getUnaryOperator(const std::string& name) const final;
    const TokenInfo& getBinaryOperator(const std::string& name) const final;

private:
    void loadPlugins();
    void loadPlugin(const std::string& path);
    static void validatePlugin(const FunctionInfo* info);

    std::unordered_map<std::string, TokenInfo> functions_;
    std::unordered_map<std::string, TokenInfo> unary_ops_;
    std::unordered_map<std::string, TokenInfo> binary_ops_;

    std::vector<void*> handles_;
};