#include <filesystem>
#include <iostream>

#include "PluginManager.hpp"

PluginManager::~PluginManager() = default;

void PluginManager::loadPluginsFromDirectory(const std::string& dirPath) {
    namespace fs = std::filesystem;
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Plugins directory not found: " << dirPath << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().extension() != ".dll") continue;

        HMODULE hmod = LoadLibraryW(entry.path().c_str());
        if (!hmod) {
            std::cerr << "Failed to load DLL: " << entry.path().filename().string() << std::endl;
            continue;
        }

        auto get_name = reinterpret_cast<const char* (*)()>(
            GetProcAddress(hmod, "get_function_name")
        );
        auto evaluate = reinterpret_cast<double (*)(double)>(
            GetProcAddress(hmod, "evaluate")
        );

        if (!get_name || !evaluate) {
            std::cerr << "DLL missing required exports: " << entry.path().filename().string() << std::endl;
            FreeLibrary(hmod);
            continue;
        }

        try {
            const char* name_cstr = get_name();
            if (!name_cstr || std::string(name_cstr).empty()) {
                std::cerr << "Empty function name in: " << entry.path().filename().string() << std::endl;
                FreeLibrary(hmod);
                continue;
            }
            std::string name(name_cstr);

            //capture function and name by value
            auto wrapper = [evaluate, name](double x) -> double {
                try {
                    return evaluate(x);
                }
                catch (std::exception& e) {
                    // throw exception with additional data for debugging
                    throw std::runtime_error("Error evaluating function " + name + ": " + e.what());
                }
            };

            auto handle = std::make_unique<PluginHandle>(hmod);
            handle->func = wrapper;
            plugins.emplace(name, std::move(handle));
            functions.emplace(name, wrapper);

            std::cout << "Loaded function: " << name << " from " << entry.path().filename().string() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error initializing plugin " << entry.path().filename().string()
                << ": " << e.what() << std::endl;
            FreeLibrary(hmod);
        }
    }
}
