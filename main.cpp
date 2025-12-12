#include "Wrapper.hpp"

struct A {
    int foo(int a, int b) {
        return b - a;
    }
};

int main() {
    A a;
    Wrapper<A, int, int, int> t(&a, &A::foo, {{"a", 12}, {"b", 11}});
    return t.execute({
            {"a", 5}
        });
}
