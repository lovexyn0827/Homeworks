#include "util.h"

template <typename T>
Filter<T>::Filter(bool (*predicate) (const T&)) : predicate(predicate) {
}

template <typename T>
bool Filter<T>::test(const T & val) {
    return this->predicate(val);
}
