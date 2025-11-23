#include "PluginManager.hpp"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <set>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#define DLOPEN(filename)    LoadLibraryA(filename)
#define DLSYM(handle, sym)  GetProcAddress((HMODULE)handle, sym)
#define DLCLOSE(handle)     FreeLibrary((HMODULE)handle)
#define DLERROR()           "LoadLibrary/GetProcAddress failed"
const char* PLUGIN_EXT = ".dll";
#else
#include <dlfcn.h>
#define DLOPEN(filename)    dlopen(filename, RTLD_NOW)
#define DLSYM(handle, sym)  dlsym(handle, sym)
#define DLCLOSE(handle)     dlclose(handle)
#define DLERROR()           dlerror()
#ifdef __APPLE__
const char* PLUGIN_EXT = ".dylib";
#else
const char* PLUGIN_EXT = ".so";
#endif
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
        if (h) DLCLOSE(h);
    }
}

void PluginManager::loadPlugins() {
    namespace fs = std::filesystem;
    fs::path pluginDir = "plugins";
    if (!fs::exists(pluginDir)) {
        throw std::runtime_error("Plugin directory 'plugins' not found");
    }

    for (const auto& entry : fs::directory_iterator(pluginDir)) {
        if (entry.path().extension() == PLUGIN_EXT) {
            try {
                loadPlugin(entry.path().string());
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to load plugin " << entry.path().filename()
                    << ": " << e.what() << "\n";
            }
        }
    }

    if (functions_.empty() && unary_ops_.empty() && binary_ops_.empty()) {
        throw std::runtime_error("No valid plugins loaded");
    }
}

void PluginManager::loadPlugin(const std::string& path) {
    void* handle = DLOPEN(path.c_str());
    if (!handle) {
        throw std::runtime_error("Failed to load plugin: " + path);
    }

    using GetInfoFunc = FunctionInfo*(*)();
    auto getInfo = reinterpret_cast<GetInfoFunc>(DLSYM(handle, "get_function_info"));
    if (!getInfo) {
        DLCLOSE(handle);
        throw std::runtime_error("Symbol 'get_function_info' not found in " + path);
    }

    const FunctionInfo* info = getInfo();
    if (!info || !info->name || !info->evaluate) {
        DLCLOSE(handle);
        throw std::runtime_error("Invalid FunctionInfo from " + path);
    }

    validatePlugin(info);

    std::string name(info->name);
    TokenInfo tokenInfo{
        info->arity,
        info->precedence,
        info->associativity,
        info->is_operator,
        info->evaluate
    };

    if (info->is_operator) {
        if (info->arity == 1) {
            if (unary_ops_.count(name)) {
                DLCLOSE(handle);
                throw std::runtime_error("Duplicate unary operator: " + name);
            }
            unary_ops_[name] = tokenInfo;
        } else if (info->arity == 2) {
            if (binary_ops_.count(name)) {
                DLCLOSE(handle);
                throw std::runtime_error("Duplicate binary operator: " + name);
            }
            binary_ops_[name] = tokenInfo;
        } else {
            DLCLOSE(handle);
            throw std::runtime_error("Operator must have arity 1 or 2: " + name);
        }
    } else {
        // function
        if (functions_.count(name)) {
            DLCLOSE(handle);
            throw std::runtime_error("Duplicate function: " + name);
        }
        functions_[name] = tokenInfo;
    }

    handles_.push_back(handle);
}

bool PluginManager::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}
bool PluginManager::hasUnaryOperator(const std::string& name) const {
    return unary_ops_.find(name) != unary_ops_.end();
}
bool PluginManager::hasBinaryOperator(const std::string& name) const {
    return binary_ops_.find(name) != binary_ops_.end();
}

const PluginManager::TokenInfo& PluginManager::getFunction(const std::string& name) const {
    auto it = functions_.find(name);
    if (it == functions_.end()) throw std::runtime_error("Function not found: " + name);
    return it->second;
}
const PluginManager::TokenInfo& PluginManager::getUnaryOperator(const std::string& name) const {
    auto it = unary_ops_.find(name);
    if (it == unary_ops_.end()) throw std::runtime_error("Unary operator not found: " + name);
    return it->second;
}
const PluginManager::TokenInfo& PluginManager::getBinaryOperator(const std::string& name) const {
    auto it = binary_ops_.find(name);
    if (it == binary_ops_.end()) throw std::runtime_error("Binary operator not found: " + name);
    return it->second;
}
