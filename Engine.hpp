#pragma once

#include <any>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Wrapper.hpp"

class Engine {
public:
    template <typename T, typename Ret, typename... Args>
    void register_command(const Wrapper<T, Ret, Args...>* wrapper, const std::string& command_name) {
        auto executor = [wrapper](const ArgList& args) -> std::any {
            return wrapper->execute(args);
        };
        command_executors[command_name] = executor;
    }

    std::any execute(const std::string& command_name, const ArgList& args) {
        auto it = command_executors.find(command_name);
        if (it == command_executors.end()) {
            throw std::invalid_argument("Command not found: " + command_name);
        }

        return it->second(args);
    }

private:
    std::unordered_map<std::string, std::function<std::any(const ArgList&)>> command_executors;
};
