#ifndef STUDENTS_H
#define STUDENTS_H
#include <array>
#include <string>
#include <utility>

class Student {
    double rightness_chance;

public:
    std::string name;

    explicit Student(double rightness_chance, const std::string& name): rightness_chance(rightness_chance), name(name) {
    }

    std::pair<double, double> solve(const std::array<double, 3> &coefs) const;
};

#endif //STUDENTS_H
