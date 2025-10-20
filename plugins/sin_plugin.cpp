#include <cmath>
#include "../src/plugin_interface.h"

struct FunctionInfo;

static double sin_eval(const double* args, size_t count) {
    if (count != 1) return 0.0;
    return std::sin(args[0]);
}

static const FunctionInfo info = {
    "sin", 1, 90, Associativity::Left, false, sin_eval
};

PLUGIN_API const FunctionInfo* get_function_info() { return &info; }