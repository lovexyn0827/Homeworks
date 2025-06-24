#include "operationhistory.h"

namespace lovexyn0827 {
namespace personnel {

OperationHistory::OperationHistory() {}

void OperationHistory::redo() {
    Operation * op = this->undoneOperations.top();
    this->operationHistory.push(op);
    op->redo();
    this->undoneOperations.pop();
}

void OperationHistory::undo() {
    Operation * op = this->operationHistory.top();
    this->undoneOperations.push(op);
    op->undo();
    this->operationHistory.pop();
}

void OperationHistory::pushOperation(Operation & op) {
    while (!this->undoneOperations.empty()) {
        delete this->undoneOperations.top();

    }
}

} // namespace personnel
} // namespace lovexyn0827
