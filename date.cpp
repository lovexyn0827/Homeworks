#include "date.h"

#include <cstdio>

Date::Date(void) {
    this->year = 1970;
    this->month = 1;
    this->day = 1;
}

Date::Date(int y, int m, int d) {
    this->year = y;
    this->month = m;
    this->day = d;
}

Date::~Date() {
    // Empty stub
}

void Date::SetYear(int y) {
    this->year = y;
}

void Date::SetMonth(int m) {
    this->month = m;
}
void Date::SetDay(int d) {
    this->day = d;
}

int Date::GetYear() {
    return this->year;
}

int Date::GetMonth() {
    return this->month;
}

int Date::GetDay() {
    return this->day;
}

Date& Date::operator()(int y, int m, int d) {
    this->year = y;
    this->month = m;
    this->day = d;
    return *this;
}

std::string Date::format() {
    char buf[16];
    sprintf(buf, "%02d-%02d-%02d", this->year, this->month, this->day);
    return buf;
}

bool operator==(Date date1, Date date2) {
    return date1.year == date2.year && date1.month == date2.month && date1.day == date2.day;
}

bool operator!=(Date date1, Date date2) {
    return date1.year != date2.year || date1.month != date2.month || date1.day != date2.day;
}

std::istream& operator>>(std::istream& in, Date& date) {
    in >> date.year;
    in.ignore();
    in >> date.month;
    in.ignore();
    in >> date.day;
    return in;
}

std::ostream& operator<<(std::ostream& out, Date& date) {
    out << date.year << '-' << date.month << '-' << date.day;
    return out;
}

bool Date::validate(int y, int m, int d) {
    if (y < 1900 || y > 9999) {
        return false;
    }

    if (m < 1 || m > 12) {
        return false;
    }

    bool leapYear = y % 400 == 0 || y % 4 == 0 && y % 100 != 0;
    static int daysOfMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    return d > 0 && d < ((leapYear && m == 2) ? 29 : daysOfMonth[m]);
}
