#ifndef OPERATION_H
#define OPERATION_H

class Operation
{
public:
    virtual void redo() = 0;
    virtual void undo() = 0;
};

#endif // OPERATION_H
