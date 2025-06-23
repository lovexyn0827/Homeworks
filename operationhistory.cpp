#include "operationhistory.h"

namespace lovexyn0827 {
namespace personnel {

OperationHistory::OperationHistory() {}

void OperationHistory::redo() {
    this->undoneOperations.push(this->operationHistory.top());
    this->operationHistory.pop();
}

void OperationHistory::undo() {

}

void OperationHistory::pushOperation(Operation & op) {

}

} // namespace personnel
} // namespace lovexyn0827
