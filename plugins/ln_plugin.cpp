#include "../src/plugin_interface.h"
#include <cmath>

//количество аргументов функции (оператора)
#define ARGC 1

PLUGIN_API PluginResult ln_eval(const double* args, size_t count) {
    if (count != ARGC) return PluginResult{0.0, "Should accept 1 argument"};
    if (args[0] <= 0.0) {
        return PluginResult{0.0, "Argument must be positive"};
    }
    return PluginResult{std::log(args[0]), nullptr};
}

static const FunctionInfo info = {
    "ln", ARGC, 90, Associativity::Left, false, ln_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}