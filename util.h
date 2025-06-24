#ifndef UTIL_H
#define UTIL_H

#define ERROR(MSG) std::cerr << (MSG) << std::endl;

template <typename T>
class Filter {
private:
    bool (*predicate) (const T&);

public:
    Filter(bool (*predicate) (const T&));
    virtual bool test(const T & val);
};

#endif // UTIL_H
