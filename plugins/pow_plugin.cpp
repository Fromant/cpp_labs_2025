#include "../src/plugin_interface.h"

#include <cmath>
#include <stdexcept>

//количество аргументов функции (оператора)
#define ARGC 2

PLUGIN_API PluginResult pow_eval(const double* args, size_t) {
    double base = args[0], exp = args[1];
    return PluginResult{std::pow(base, exp), nullptr};
}

static const FunctionInfo info = {
    "^", ARGC, 90, Associativity::Right, true, pow_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}