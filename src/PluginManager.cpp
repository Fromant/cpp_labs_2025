#include "PluginManager.hpp"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <set>

#ifdef _WIN32
#include <windows.h>
#else
#error "Windows only"
#endif

void PluginManager::validatePlugin(FunctionInfo const* info) {
    std::string name(info->name);
    if (name.empty()) {
        throw std::runtime_error("Plugin name is empty");
    }
    static const std::set<std::string> reserved = {"(", ")", ","};
    if (reserved.count(name)) {
        throw std::runtime_error("Plugin cannot register reserved symbol: " + name);
    }
    if (std::isdigit(name[0])) {
        throw std::runtime_error("Plugin name cannot start with a digit: " + name);
    }
    if (info->is_operator && name.length() != 1) {
        throw std::runtime_error("Plugin operator can only have one char as a name: " + name);
    }
}

PluginManager::PluginManager() {
    loadPlugins();
}

PluginManager::~PluginManager() {
    for (void* h : handles_) {
        FreeLibrary(static_cast<HMODULE>(h));
    }
}

void PluginManager::loadPlugins() {
    namespace fs = std::filesystem;
    fs::path pluginDir = "plugins";
    if (!fs::exists(pluginDir)) {
        throw std::runtime_error("Plugin directory 'plugins' not found");
    }

    for (const auto& entry : fs::directory_iterator(pluginDir)) {
        if (entry.path().extension() == ".dll") {
            try {
                loadPlugin(entry.path().string());
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to load plugin " << entry.path().filename()
                    << ": " << e.what() << "\n";
            }
        }
    }

    if (registry_.empty()) {
        throw std::runtime_error("No valid plugins loaded");
    }
}

void PluginManager::loadPlugin(const std::string& path) {
    HMODULE handle = LoadLibraryA(path.c_str());
    if (!handle) {
        throw std::runtime_error("LoadLibrary failed");
    }

    // typedef const FunctionInfo* (*GetInfoFunc)();
    using GetInfoFunc = FunctionInfo*(*)();
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

    validatePlugin(info);

    std::string name(info->name);

    // Check for duplicates
    if (registry_.count(name)) {
        FreeLibrary(handle);
        throw std::runtime_error("Duplicate token name: " + name);
    }

    registry_[name] = TokenInfo{
            info->arity,
            info->precedence,
            info->associativity,
            info->is_operator,
            info->evaluate
        };

    handles_.push_back(handle);
}

bool PluginManager::hasToken(const std::string& name) const {
    return registry_.count(name) > 0;
}

const PluginManager::TokenInfo& PluginManager::getTokenInfo(const std::string& name) const {
    auto it = registry_.find(name);
    if (it == registry_.end()) {
        throw std::runtime_error("Unknown token: " + name);
    }
    return it->second;
}
