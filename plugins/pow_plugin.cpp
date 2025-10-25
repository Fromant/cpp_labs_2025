#include "../src/plugin_interface.h"

#include <cmath>
#include <stdexcept>

static double pow_eval(const double* args, size_t) {
    double base = args[0], exp = args[1];
    if (base < 0 && std::floor(exp) != exp)
        throw std::domain_error("pow: negative base with non-integer exponent");
    return std::pow(base, exp);
}

static const FunctionInfo info = {
    "^", 2, 80, Associativity::Right, true, pow_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}