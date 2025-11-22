#pragma once

#include <cstddef> //for size_t

#ifdef _WIN32
#  define PLUGIN_API extern "C" __declspec(dllexport)
#else
#error "Only windows supported"
#endif

extern "C" {
    enum class Associativity {
        Left,
        Right
    };

    struct PluginResult {
        double value;
        const char* error;
    };

    struct FunctionInfo {
        const char* name;
        int arity;
        int precedence;
        Associativity associativity;
        bool is_operator;
        PluginResult (*evaluate)(const double* args, size_t count);
    };

    PLUGIN_API const FunctionInfo* get_function_info();
}
