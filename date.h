#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

class Date
{
public:
    Date(void);
    Date(int y, int m, int d);

public:
    virtual ~Date();

    void SetYear(int y);
    void SetMonth(int m);
    void SetDay(int d);

    int GetYear();
    int GetMonth();
    int GetDay();

    Date& operator()(int y, int m, int d);

    std::string format();

    friend bool operator==(Date date1, Date date2);
    friend bool operator!=(Date date1, Date date2);

    friend std::istream& operator>>(std::istream& in, Date& date);
    friend std::ostream& operator<<(std::ostream& out, Date& date);

private:
    int year;
    int month;
    int day;

private:
    bool validate(int y, int m, int d);
};

#endif // DATE_H
