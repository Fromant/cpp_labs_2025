#include <iostream>
#include <filesystem>
#include <stdexcept>

#include "PluginManager.hpp"
#include "plugin_interface.h"

PluginManager::PluginManager() {
    loadPlugins();
}

PluginManager::~PluginManager() {
    for (void* handle : handles_) {
        FreeLibrary(static_cast<HMODULE>(handle));
    }
}

void PluginManager::loadPlugins() {
    namespace fs = std::filesystem;
    fs::path pluginDir = "plugins";
    if (!fs::exists(pluginDir) || !fs::is_directory(pluginDir)) {
        std::cerr << "Plugin directory 'plugins' not found. Skipping plugins.\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(pluginDir)) {
        if (entry.path().extension() == ".dll") {
            try {
                loadPlugin(entry.path().string());
            } catch (const std::exception& e) {
                std::cerr << "Failed to load plugin " << entry.path().filename()
                          << ": " << e.what() << "\n";
            }
        }
    }
}

void PluginManager::loadPlugin(const std::string& path) {
    HMODULE handle = LoadLibraryA(path.c_str());
    if (!handle) {
        throw std::runtime_error("LoadLibrary failed");
    }

    using GetInfoFunc = const FunctionInfo* (*)();
    auto getInfo = reinterpret_cast<GetInfoFunc>(
        GetProcAddress(handle, "get_function_info")
    );

    if (!getInfo) {
        FreeLibrary(handle);
        throw std::runtime_error("Symbol 'get_function_info' not found");
    }

    const FunctionInfo* info = getInfo();
    if (!info || !info->name || !info->evaluate) {
        FreeLibrary(handle);
        throw std::runtime_error("Invalid FunctionInfo");
    }

    const RegisteredFunction rf{
        info->arity,
        info->precedence,
        info->is_operator,
        info->associativity,
        info->evaluate
    };

    registry_[std::string(info->name)] = rf;
    handles_.push_back(handle);
}

bool PluginManager::hasFunction(const std::string& name) const {
    return registry_.count(name) > 0;
}

const PluginManager::RegisteredFunction& PluginManager::getFunction(const std::string& name) const {
    auto it = registry_.find(name);
    if (it == registry_.end()) {
        throw std::runtime_error("Function not found: " + name);
    }
    return it->second;
}