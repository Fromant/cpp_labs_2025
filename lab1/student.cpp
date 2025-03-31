#include "student.h"

#include "lab1.h"

std::pair<double, double> Student::solve(const std::array<double, 3> &coefs) const {
    if (rightness_chance <= 0) {
        //Bad student
        return std::make_pair(0, NAN);
    }
    const auto solution = solve_quadratic_equation(coefs);
    if (rightness_chance >= 1) {
        //Good student
        return solution;
    }
    //else, avg student
    float rand_val = float(rand()) / float(RAND_MAX); // random float between 0 and 1

    if (rand_val < rightness_chance) {
        // if less, return right solution
        return solution;
    }
    if (rand_val < rightness_chance * 2) {
        // semi right solution, like student too lazy to count second root
        return {solution.first,NAN};
    }
    // else, return horrible solution
    return {solution.first - 1, -solution.second};
}
