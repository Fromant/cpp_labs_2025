#pragma once

#include <string>
#include <functional>
#include <map>

#include "plugin_interface.h"

#ifdef _WIN32
#include <windows.h>
#else
    #error "Only Windows is supported for plugins (DLLs)"
#endif

struct FunctionInfo;

using FunctionMap = std::map<std::string, std::function<double(double)>>;

class PluginManager {
public:
    struct RegisteredFunction {
        int arity;
        int precedence;
        bool is_operator;
        Associativity associativity;
        double (*evaluate)(const double*, size_t);
    };

    PluginManager();
    ~PluginManager();

    void loadPlugins();
    bool hasFunction(const std::string& name) const;
    const RegisteredFunction& getFunction(const std::string& name) const;

private:
    void loadPlugin(const std::string& path);
    std::unordered_map<std::string, RegisteredFunction> registry_;

    std::vector<void*> handles_; // HMODULE
};
