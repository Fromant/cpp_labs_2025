#include "../src/plugin_interface.h"

static double add(const double* args, size_t) {
    return args[0] + args[1];
}

static const FunctionInfo info = {
    "+", 2, 60, Associativity::Left, true, add
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}
