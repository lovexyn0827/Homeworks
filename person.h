#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

#include "Date.h"

class Person
{
public:
    Person();

public:
    virtual ~Person();

public:
    void operator()(std::string id, std::string name, std::string sex,
            std::string phoneNo, std::string address, Date& birthday,
            std::string employeeNo, std::string  department, std::string post, double salary);

    friend bool operator!=(Person person1, Person person2);
    friend bool operator==(Person person1, Person person2);
    friend bool operator<(Person person1, Person person2);
    friend bool operator>(Person person1, Person person2);
    friend bool operator<=(Person person1, Person person2);
    friend bool operator>=(Person person1, Person person2);

    std::string GetId() const;
    std::string GetName() const;
    std::string GetSex() const;
    std::string GetPhoneNo() const;
    std::string GetAddress() const;
    Date GetBirthday() const;
    std::string GetEmployeeNo() const;
    std::string GetDepartment() const;
    std::string GetPost() const;
    double GetSalary() const;

    void SetId(std::string id);
    void SetName(std::string name);
    void SetSex(std::string sex);
    void SetPhoneNo(std::string phoneNo);
    void SetAddress(std::string address);
    void SetBirthday(Date birthday);
    void SetEmployeeNo(std::string employeeNo);
    void SetDepartment(std::string department);
    void SetPost(std::string post);
    void SetSalary(double salary);

    friend std::istream& operator>>(std::istream& in, Person& person);
    friend std::ostream& operator<<(std::ostream& out, Person& person);

protected:
    std::string id;
    std::string name;
    std::string sex;
    std::string phoneNo;
    std::string address;
    Date birthday;
    std::string employeeNo;
    std::string department;
    std::string post;
    double salary;
};

#endif // PERSON_H
