#include <cmath>
#include <stdexcept>

extern "C" __declspec(dllexport) const char* get_function_name() {
    return "sin";
}

extern "C" __declspec(dllexport) double evaluate(double x) {
    // sin expects radians
    return std::sin(x);
}