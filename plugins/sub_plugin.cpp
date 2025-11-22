#include "../src/plugin_interface.h"


//количество аргументов функции (оператора)
#define ARGC 2

PLUGIN_API PluginResult sub_eval(const double* args, size_t) {
    return PluginResult{args[0] - args[1], nullptr};
}

static const FunctionInfo info = {
    "-", ARGC, 60, Associativity::Left, true, sub_eval
};

PLUGIN_API const FunctionInfo* get_function_info() {
    return &info;
}