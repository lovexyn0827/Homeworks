#ifndef OPERATION_H
#define OPERATION_H

#include "person.h"
#include "personset.h"

#include <list>

class Operation
{
public:
    virtual ~Operation();

    virtual void redo(PersonSet & storage) const = 0;
    virtual void undo(PersonSet & storage) const = 0;
};

class DeleteOperation : public Operation {
private:
    const std::list<Person> & deleted;
    const std::list<int> & deletedIndices;

public:
    DeleteOperation(const std::list<Person> & toDelete, const std::list<int> & indices);
    virtual ~DeleteOperation();

    virtual void redo(PersonSet & storage) const;
    virtual void undo(PersonSet & storage) const;
};

class AddOperation : public Operation {
private:
    const Person added;
    int index;

public:
    AddOperation(const Person & added, int index);
    virtual ~AddOperation();

    virtual void redo(PersonSet & storage) const;
    virtual void undo(PersonSet & storage) const;
};

class ModifyOperation : public Operation {
private:
    const Person original;
    const Person modified;
    int index;

public:
    ModifyOperation(const Person & original, const Person & modified, int index);
    virtual ~ModifyOperation();

    virtual void redo(PersonSet & stroage) const;
    virtual void undo(PersonSet & stroage) const;
};

#endif // OPERATION_H
