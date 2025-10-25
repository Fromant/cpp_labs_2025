#include <iostream>
#include <string>

#include "src/Calculator.hpp"

int main() {
    try {
        std::string input;
        std::getline(std::cin, input);
        Calculator calc;
        std::cout << calc.evaluate(input) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}