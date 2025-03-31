#pragma once

#include <array>
#include <cmath>

inline std::pair<double, double> solve_quadratic_equation(const std::array<double, 3>& equation) {
    // Count discriminant
    double discr = equation[1]*equation[1]-4*equation[0]*equation[2];

    if(discr < 0) {
        return {NAN, NAN};
    }

    if(discr == 0) {
        double root = (-equation[1]+std::sqrt(discr))/(2*equation[0]);
        return {root, NAN};
    }

    // 2 roots
    double sqrt_discr = std::sqrt(discr);
    double denominator = 2*equation[0];
    double root1 = (-equation[1] + sqrt_discr) / denominator;
    double root2 = (-equation[1] - sqrt_discr) / denominator;
    return {root1, root2};
}