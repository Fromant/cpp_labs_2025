#ifndef TEACHER_H
#define TEACHER_H
#include <map>
#include <queue>

#include "letter.h"

class Teacher {
    std::queue<Letter> queue;
    std::map<std::string, int> log;

public:
    void add_letter(const Letter &l) { queue.push(l); }

    void check_all_letters();

    void print_table();
};

#endif //TEACHER_H
