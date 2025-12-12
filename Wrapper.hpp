#pragma once

#include <any>
#include <functional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

// T is a object type
// Ret is a function's return type
// Args is a function's parameters types
template <typename T, typename Ret, typename... Args>
class Wrapper {
    static constexpr size_t ARG_COUNT = sizeof...(Args);

    using ArgMap = std::unordered_map<std::string, std::any>;
    using ArgList = std::vector<std::pair<std::string, std::any>>;
    using Func = Ret (T::*)(Args...);

    T* const _obj;
    const Func _func;

    const ArgList argNames;
    std::array<std::type_index, ARG_COUNT> argTypes = { typeid(Args)... };

    template <std::size_t... Indices>
    Ret call_with_indices(const std::array<std::any, ARG_COUNT>& args, std::index_sequence<Indices...>) {
        return (_obj->*_func)(std::any_cast<Args>(args[Indices])...);
    }

public:
    Wrapper(T* const object, Func function, const ArgList& argList) :
        _obj(object), _func(function), argNames(argList) {
        if (argList.size() != ARG_COUNT) {
            throw std::invalid_argument("Wrong number of arguments");
        }
    }

    Ret execute(const ArgList& list) {
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

        return call_with_indices(args, std::make_index_sequence<ARG_COUNT>());
    }
};
