#include <iostream>

#include "Engine.hpp"
#include "Wrapper.hpp"

struct A {
    int foo(int a, int b) {
        return b - a;
    }
};

int main() {
    A a;
    Wrapper t(&a, &A::foo, {{"a", 12}, {"b", 11}});
    std::cout << t.execute({
            {"a", 5}
        }) << std::endl;

    Engine e;
    e.register_command(&t, "t");

    std::cout << std::any_cast<int>(e.execute("t", {{"b", 16}})) << std::endl;
}
