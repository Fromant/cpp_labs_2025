#include "../src/plugin_interface.h"
#include <cmath>

//количество аргументов функции (оператора)
#define ARGC 1

PLUGIN_API PluginResult cos_eval(const double* args, size_t count) {
    if (count != ARGC) return PluginResult{0.0, "Should accept 1 argument"};
    return PluginResult{std::cos(args[0]), nullptr};
}

static const FunctionInfo info = {
    "cos", ARGC, 90, Associativity::Left, false, cos_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}