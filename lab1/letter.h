#ifndef LETTER_H
#define LETTER_H
#include <array>
#include <string>

struct Letter {
    std::array<double, 3> equation;
    std::pair<double, double> solution;
    std::string student_name;
};


#endif //LETTER_H
