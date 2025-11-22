#include "../src/plugin_interface.h"

#include <stdexcept>

//количество аргументов функции (оператора)
#define ARGC 2

PLUGIN_API PluginResult div_eval(const double* args, size_t count) {
    if (count != ARGC) return PluginResult{0.0, "Should accept 2 arguments"};
    if (args[1] == 0.0) return PluginResult{0.0, "Division by zero"};
    return PluginResult{args[0] / args[1], nullptr};
}

static const FunctionInfo info = {
        "/", ARGC, 70, Associativity::Left, true, div_eval
    };

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}
