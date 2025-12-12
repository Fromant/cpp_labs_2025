#pragma once

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Wrapper.hpp"

class Engine {
public:
    template <typename T, typename Ret, typename... Args>
    void register_command(const std::string& name, const Wrapper<T, Ret, Args...>& wrapper) {
        wrappers.emplace(name, std::make_unique<Wrapper<T, Ret, Args...>>(wrapper));
    }

    template <typename Obj, typename T, typename Ret, typename... Args>
    void register_command(
        const std::string& name,
        Obj obj,
        Ret(T::*func)(Args...),
        const WrapperBase::ArgList& argList
    ) {
        wrappers.emplace(name, std::make_unique<Wrapper<T, Ret, Args...>>(obj, func, argList));
    }


    std::any execute(const std::string& command_name, const WrapperBase::ArgList& args) {
        auto it = wrappers.find(command_name);
        if (it == wrappers.end()) {
            throw std::invalid_argument("Command not found: " + command_name);
        }

        return it->second->execute(args);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<WrapperBase>> wrappers;
};
