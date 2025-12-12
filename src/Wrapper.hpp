#pragma once

#include <any>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "WrapperBase.hpp"


// T is a object type
// Ret is a function's return type
// Args is a function's parameters types
template <typename T, typename Ret, typename... Args>
class Wrapper : public WrapperBase {
    static constexpr size_t ARG_COUNT = sizeof...(Args);

    using ArgMap = std::unordered_map<std::string, std::any>;
    using Func = Ret (T::*)(Args...);

    std::unique_ptr<T> _obj;
    Func _func;

    ArgList argNames;
    std::array<std::type_index, ARG_COUNT> argTypes = {typeid(Args)...};

    template <typename R = Ret, std::size_t... Indices>
    std::enable_if_t<!std::is_void_v<R>, std::any>
    invoke_function(const std::array<std::any, ARG_COUNT>& args, std::index_sequence<Indices...>) const {
        return ((*_obj).*_func)(std::any_cast<Args>(args[Indices])...);
    }

    // Helper for void return
    template <typename R = Ret, std::size_t... Indices>
    std::enable_if_t<std::is_void_v<R>, std::any>
    invoke_function(const std::array<std::any, ARG_COUNT>& args, std::index_sequence<Indices...>) const {
        ((*_obj).*_func)(std::any_cast<Args>(args[Indices])...);
        return {}; // or std::any{}
    }

public:
    Wrapper(const T& object, Func function, const ArgList& argList) :
        _obj(std::make_unique<T>(object)), _func(function), argNames(argList) {
        if (argList.size() != ARG_COUNT) {
            throw std::invalid_argument("Wrong number of arguments");
        }
    }

    Wrapper(const Wrapper& other): _obj(std::make_unique<T>(*other._obj)), _func(other._func),
                                   argNames(other.argNames) {}

    Wrapper(Wrapper&& other) noexcept : _obj(std::move(other._obj)), _func(std::move(other._func)),
                                        argNames(std::move(other.argNames)) {}

    // Copy assignment
    Wrapper& operator=(const Wrapper& other) {
        if (this != &other) {
            _obj = std::make_unique<T>(*other._obj);
            _func = other._func;
            argNames = other.argNames;
        }
        return *this;
    }

    // Move assignment
    Wrapper& operator=(Wrapper&& other) noexcept {
        if (this != &other) {
            _obj = std::move(other._obj);
            _func = other._func;
            argNames = std::move(other.argNames);
        }
        return *this;
    }


    std::any execute(const ArgList& list) const override {
        if (list.size() > ARG_COUNT) {
            throw std::invalid_argument("Too many arguments");
        }
        std::array<std::any, ARG_COUNT> args;

        for (size_t i = 0; i < ARG_COUNT; i++) {
            const auto& [name, def] = argNames[i];
            auto it = std::find_if(list.begin(), list.end(), [&name](const std::pair<std::string, std::any>& arg) {
                return arg.first == name;
            });

            if (it == list.end()) {
                args[i] = def;
            }
            else {
                if (it->second.type() != argTypes[i]) {
                    throw std::invalid_argument("Type mismatch for argument: " + name);
                }
                args[i] = it->second;
            }
        }

        return invoke_function(args, std::make_index_sequence<ARG_COUNT>{});
    }
};
