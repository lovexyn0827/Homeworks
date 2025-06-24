#include "personset.h"
#include "util.h"

#include <fstream>

PersonSet::PersonSet(void) {
    // Empty stub
}

PersonSet::~PersonSet() {
    // Empty stub
}

bool PersonSet::checkForDuplication(const Person& against) {
    for (PersonSet::iterator itr = this->begin(); itr != this->end(); itr++) {
        if (*itr == against) {
            return true;
        }
    }

    return false;
}

bool PersonSet::push_back(const Person& person) {
    if (this->checkForDuplication(person)) {
        return false;
    } else {
        this->deque<Person>::push_back(person);
        return true;
    }
}

bool PersonSet::push_front(const Person& person) {
    if (this->checkForDuplication(person)) {
        return false;
    } else {
        this->deque<Person>::push_front(person);
        return true;
    }
}

std::deque<Person>::iterator PersonSet::insert(std::deque<Person>::iterator i, const Person& person) {
    if (this->checkForDuplication(person)) {
        return i;
    } else {
        this->deque<Person>::insert(i, person);
        return i + 1;
    }
}

int PersonSet::ReadFromFile(const char filename[]) {
    std::ifstream fin(filename, std::ios::in);
    if (fin.fail()) {
        ERROR("Failed to open data file!")
        return 0;
    }

    int cnt = 0;
    while (fin.good() && !fin.eof()) {
        Person p;
        fin >> p;
        if (this->push_back(p)) {
            cnt++;
        } else {
           ERROR("Omitting duplicated record.");
        }
    }

    if (!this->empty()) {
        this->pop_back();
        cnt--;
    }

    return cnt;
}

int PersonSet::WriteToFile(const char filename[]) {
    std::ofstream fout(filename, std::ios::out | std::ios::trunc);
    if (fout.fail()) {
        ERROR("Failed to open data file!")
        return 0;
    }

    int cnt = 0;
    for (PersonSet::iterator itr = this->begin(); itr != this->end(); itr++) {
        fout << *itr << std::endl;
        cnt++;
    }

    return cnt;
}
