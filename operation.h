#ifndef OPERATION_H
#define OPERATION_H

#include "person.h"

class Operation
{
public:
    virtual ~Operation();

    virtual void redo() = 0;
    virtual void undo() = 0;
};

class DeleteOperation : public Operation {
private:
    const Person & toDelete;

public:
    DeleteOperation(const Person & toDelete);
    virtual ~DeleteOperation();

    virtual void redo();
    virtual void undo();
};

#endif // OPERATION_H
