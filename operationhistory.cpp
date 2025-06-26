#include "operationhistory.h"

OperationHistory::OperationHistory(PersonSet & storage) : storage(storage) {}

bool OperationHistory::redo() {
    const Operation * op = this->undoneOperations.top();
    this->operationHistory.push(op);
    op->redo(this->storage);
    this->undoneOperations.pop();
    return !this->undoneOperations.empty();
}

bool OperationHistory::undo() {
    const Operation * op = this->operationHistory.top();
    this->undoneOperations.push(op);
    op->undo(this->storage);
    this->operationHistory.pop();
    return !this->operationHistory.empty();
}

void OperationHistory::pushOperation(const Operation * op) {
    while (!this->undoneOperations.empty()) {
        delete this->undoneOperations.top();
        this->undoneOperations.pop();
    }

    this->operationHistory.push(op);
}

void OperationHistory::reset() {
    while (!this->operationHistory.empty()) {
        delete this->undoneOperations.top();
        this->undoneOperations.pop();
    }

    while (!this->undoneOperations.empty()) {
        delete this->undoneOperations.top();
        this->undoneOperations.pop();
    }
}
