#include "../src/plugin_interface.h"

#include <stdexcept>

static double div(const double* args, size_t) {
    if (args[1] == 0.0) throw std::domain_error("Division by zero");
    return args[0] / args[1];
}

static const FunctionInfo info = {
    "/", 2, 70, Associativity::Left, true, div
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}