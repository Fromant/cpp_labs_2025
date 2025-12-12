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

    template <typename T, typename Ret, typename... Args, typename... CtorArgs>
    void register_command(const std::string& name, CtorArgs&&... args) {
        wrappers.emplace(name, std::make_unique<Wrapper<T, Ret, Args...>>(std::forward<CtorArgs>(args)...));
    }

    void register_command(const std::string& name, std::unique_ptr<WrapperBase>&& wrapper) {
        wrappers.emplace(name, std::move(wrapper));
    }

    template <typename T, typename Ret, typename... Args>
    void register_command(const std::string& name, Wrapper<T, Ret, Args...>&& wrapper) {
        wrappers.emplace(name, std::make_unique<Wrapper<T, Ret, Args...>>(std::move(wrapper)));
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
