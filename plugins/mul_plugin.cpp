#include "../src/plugin_interface.h"

static double mul(const double* args, size_t) {
    return args[0] * args[1];
}

static const FunctionInfo info = {
    "*", 2, 70, Associativity::Left, true, mul
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}