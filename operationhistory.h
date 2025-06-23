#ifndef OPERATIONHISTORY_H
#define OPERATIONHISTORY_H

#include <stack>
#include "operation.h"

namespace lovexyn0827 {
namespace personnel {

class OperationHistory
{
private:
    std::stack<Operation*> operationHistory;
    std::stack<Operation*> undoneOperations;
public:
    OperationHistory();

    void undo();
    void redo();
    void pushOperation(Operation & op);
};

} // namespace personnel
} // namespace lovexyn0827

#endif // OPERATIONHISTORY_H
