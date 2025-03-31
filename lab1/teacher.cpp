#include "teacher.h"

#include <iostream>
#include <ostream>

#include "lab1.h"


void Teacher::check_all_letters() {
    while (!queue.empty()) {
        const auto &letter = queue.front();
        if (solve_quadratic_equation(letter.equation) == letter.solution) {
            log[letter.student_name]++;
        } else {
            // ensure student is added to log
            log.try_emplace(letter.student_name, 0);
        }
        queue.pop();
    }
}

void Teacher::print_table() {
    std::cout << "name: right answer count" << std::endl;
    for (const auto &entry: log) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}
