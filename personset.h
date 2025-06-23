#ifndef PERSONSET_H
#define PERSONSET_H

#include <deque>
#include "person.h"

class PersonSet : public std::deque<Person>
{
public:
    PersonSet(void);

public:
    virtual ~PersonSet(void);

public:
    bool push_back(const Person& person);
    bool push_front(const Person& person);

    std::deque<Person>::iterator insert(std::deque<Person>::iterator i, const Person& person);

    int ReadFromFile(const char filename[]);
    int WriteToFile(const char filename[]);

private:
    bool checkForDuplication(const Person& against);
};

#endif // PERSONSET_H
