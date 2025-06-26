#ifndef OPERATIONHISTORY_H
#define OPERATIONHISTORY_H

#include <stack>
#include "operation.h"
#include "personset.h"

class OperationHistory
{
private:
    PersonSet & storage;

    std::stack<const Operation*> operationHistory;
    std::stack<const Operation*> undoneOperations;
public:
    OperationHistory(PersonSet & storage);

    bool undo();
    bool redo();
    void pushOperation(const Operation * op);
    void reset();
};

#endif // OPERATIONHISTORY_H
