#include "operation.h"

Operation::~Operation() {
}

DeleteOperation::DeleteOperation(const std::list<Person> & toDelete, const std::list<int> & indices)
    : deleted(toDelete), deletedIndices(indices) {
}

DeleteOperation::~DeleteOperation() {
    delete &(this->deleted);
    delete &(this->deletedIndices);
}

void DeleteOperation::redo(PersonSet & storage) const {
    std::list<int>::const_iterator affectedIdxItr = this->deletedIndices.begin();
    while (affectedIdxItr != this->deletedIndices.end()) {
        storage.erase(storage.begin() + *affectedIdxItr);
        affectedIdxItr++;
    }
}

void DeleteOperation::undo(PersonSet & storage) const {
    std::list<Person>::const_reverse_iterator affectedItr = this->deleted.rbegin();
    std::list<int>::const_reverse_iterator affectedIdxItr = this->deletedIndices.rbegin();
    while (affectedIdxItr != this->deletedIndices.rend()) {
        storage.insert(storage.begin() + *affectedIdxItr, *affectedItr);
        affectedItr++;
        affectedIdxItr++;
    }
}

AddOperation::AddOperation(const Person & added, int index) : added(added), index(index) {
}

AddOperation::~AddOperation() {
}

void AddOperation::redo(PersonSet & storage) const {
    storage.insert(storage.begin() + this->index, this->added);
}

void AddOperation::undo(PersonSet & storage) const {
    storage.erase(storage.begin() + this->index);
}

ModifyOperation::ModifyOperation(const Person & original, const Person & modified, int index)
    : original(original), modified(modified), index(index) {
}

ModifyOperation::~ModifyOperation() {
}

void ModifyOperation::redo(PersonSet & storage) const {
    *(storage.begin() + this->index) = this->modified;
}

void ModifyOperation::undo(PersonSet & storage) const {
    *(storage.begin() + this->index) = this->original;
}
