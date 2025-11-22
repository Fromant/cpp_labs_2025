#include <cmath>
#include "../src/plugin_interface.h"

//количество аргументов функции (оператора)
#define ARGC 1

PLUGIN_API PluginResult sin_eval(const double* args, size_t count) {
    if (count != 1) return PluginResult{0.0, "Should accept 1 argument"};
    return PluginResult{std::sin(args[0]), nullptr};
}

static const FunctionInfo info = {
    "sin", ARGC, 90, Associativity::Left, false, sin_eval
};

PLUGIN_API const FunctionInfo* get_function_info() { return &info; }