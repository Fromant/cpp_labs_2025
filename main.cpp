#include <iostream>

#include "Engine.hpp"
#include "src/Wrapper.hpp"

struct A {
    int f(int a, int b) { return a + b; }
};

int main() {
    A obj;
    Wrapper w1(obj, &A::f, {{"a",0},{"b",0}});

    Engine e;
    e.register_command("w1", w1);
    e.register_command("w2", std::move(w1));

    std::cout << std::any_cast<int>(e.execute("w1", {{"b", 16}})) << std::endl;
}
