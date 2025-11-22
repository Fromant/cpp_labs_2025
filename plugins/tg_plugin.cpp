#include "../src/plugin_interface.h"
#include <cmath>
#include <stdexcept>

//количество аргументов функции (оператора)
#define ARGC 1

PLUGIN_API PluginResult tan_eval(const double* args, size_t count) {
    if (count != ARGC) return PluginResult{0.0, "Should accept 1 argument"};
    double x = args[0];
    return PluginResult{std::tan(x), nullptr};
}

static const FunctionInfo info = {
    "tg", ARGC, 90, Associativity::Left, false, tan_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}