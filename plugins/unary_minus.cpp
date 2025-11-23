#include "../src/plugin_interface.h"
#include <cmath>
#include <stdexcept>

#define ARGC 1

PLUGIN_API PluginResult uminus_eval(const double* args, size_t count) {
    if (count != ARGC) {
        return PluginResult{0.0, "Unary minus: expected 1 argument"};
    }
    return PluginResult{-args[0], nullptr};
}

static const FunctionInfo info = {
    "-", ARGC, 80, Associativity::Right, true, uminus_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}