#pragma once

#ifdef _WIN32
#  define PLUGIN_API extern "C" __declspec(dllexport)
#else
#error "Only windows supported"
#endif

enum class Associativity {
    Left,
    Right
};

struct FunctionInfo {
    const char* name;
    int arity;
    int precedence;
    Associativity associativity;
    bool is_operator;
    double (*evaluate)(const double* args, size_t count);
};

PLUGIN_API const FunctionInfo* get_function_info();