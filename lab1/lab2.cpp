#include <iostream>
#include <array>
#include <cmath>
#include <fstream>
#include <vector>

#include "student.h"
#include "teacher.h"

std::vector<std::array<double, 3> > read_polynoms_from_file(const std::string& filename) {
    std::vector<std::array<double, 3> > to_return{};
    std::ifstream input(filename);

    if (!input.is_open()) {
        std::cerr << "Error: Could not open input file" << std::endl;
        exit(1);
    }

    while (!input.eof()) {
        double a, b, c;
        input >> a >> b >> c;
        to_return.emplace_back(std::array{a, b, c});
    }

    return to_return;
}

int main() {
    std::cout << "Lab 2 by Krutyanscky RI" << std::endl;

    std::vector<Student> students{
        Student{0.7, "Peter"},
        Student{0.3, "Siemens"},
        Student{1.0, "Steve Jops"},
        Student{0.0, "Scibidi Dobidi"}
    };

    Teacher teacher;

    auto tasks = read_polynoms_from_file("input.txt");

    // solve equations and send them to teacher
    for (const auto &student: students) {
        for (const auto& task : tasks) {
            auto answer = student.solve(task);
            teacher.add_letter({task, answer, student.name});
        }
    }

    teacher.check_all_letters();
    teacher.print_table();

    return 0;
}
