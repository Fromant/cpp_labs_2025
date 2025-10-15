#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
#else
    #error "Only Windows is supported for plugins (DLLs)"
#endif

using FunctionMap = std::map<std::string, std::function<double(double)>>;

class PluginManager {
    struct PluginHandle {
        HMODULE module;
        std::function<double(double)> func;
        PluginHandle(HMODULE m) : module(m) {}
        ~PluginHandle() { if (module) FreeLibrary(module); }
        PluginHandle(const PluginHandle&) = delete;
        PluginHandle& operator=(const PluginHandle&) = delete;
    };

    std::map<std::string, std::unique_ptr<PluginHandle>> plugins;
    FunctionMap functions;

public:
    void loadPluginsFromDirectory(const std::string& dirPath);
    const FunctionMap& getFunctions() const { return functions; }
    ~PluginManager();
};

