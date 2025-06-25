#include "person.h"

Person::Person() {
    this->id = "114514";
}

Person::~Person() {
    // Empty Stub
}

void Person::operator()(std::string id, std::string name, std::string sex,
        std::string phoneNo, std::string address, Date& birthday,
        std::string employeeNo, std::string  department, std::string post, double salary) {
    this->id = id;
    this->name = name;
    this->sex = sex;
    this->phoneNo = phoneNo;
    this->address = address;
    this->birthday = birthday;
    this->employeeNo = employeeNo;
    this->department = department;
    this->post = post;
    this->salary = salary;
}

bool operator!=(Person person1, Person person2) {
    return person1.id != person2.id ||
           person1.name != person2.name ||
           person1.sex != person2.sex ||
           person1.phoneNo != person2.phoneNo ||
           person1.address != person2.address ||
           person1.birthday != person2.birthday ||
           person1.employeeNo != person2.employeeNo ||
           person1.department != person2.department ||
           person1.post != person2.post ||
           person1.salary != person2.salary;
}

bool operator==(Person person1, Person person2) {
    return person1.id == person2.id &&
           person1.name == person2.name &&
           person1.sex == person2.sex &&
           person1.phoneNo == person2.phoneNo &&
           person1.address == person2.address &&
           person1.birthday == person2.birthday &&
           person1.employeeNo == person2.employeeNo &&
           person1.department == person2.department &&
           person1.post == person2.post &&
           person1.salary == person2.salary;
}

bool operator<(Person person1, Person person2) {
    return person1.id < person2.id;
}

bool operator>(Person person1, Person person2) {
    return person1.id > person2.id;
}

bool operator<=(Person person1, Person person2) {
    return person1.id <= person2.id;
}

bool operator>=(Person person1, Person person2) {
    return person1.id >= person2.id;
}

std::string Person::GetId() const {
    return this->id;
}

std::string Person::GetName() const {
    return this->name;
}

std::string Person::GetSex() const {
    return this->sex;
}

std::string Person::GetPhoneNo() const {
    return this->phoneNo;
}

std::string Person::GetAddress() const {
    return this->address;
}

Date Person::GetBirthday() const {
    return this->birthday;
}

std::string Person::GetEmployeeNo() const {
    return this->employeeNo;
}

std::string Person::GetDepartment() const {
    return this->department;
}

std::string Person::GetPost() const {
    return this->post;
}

double Person::GetSalary() const {
    return this->salary;
}

void Person::SetId(std::string id) {
    this->id = id;
}

void Person::SetName(std::string name) {
    this->name = name;
}

void Person::SetSex(std::string sex) {
    this->sex = sex;
}

void Person::SetPhoneNo(std::string phoneNo) {
    this->phoneNo = phoneNo;
}

void Person::SetAddress(std::string address) {
    this->address = address;
}

void Person::SetBirthday(Date birthday) {
    this->birthday = birthday;
}

void Person::SetEmployeeNo(std::string employeeNo) {
    this->employeeNo = employeeNo;
}

void Person::SetDepartment(std::string department) {
    this->department = department;
}

void Person::SetPost(std::string post) {
    this->post = post;
}

void Person::SetSalary(double salary) {
    this->salary = salary;
}

std::istream& operator>>(std::istream& in, Person& person) {
    in >> person.id;
    in >> person.name;
    in >> person.sex;
    in >> person.birthday;
    in >> person.address;
    in >> person.phoneNo;
    in >> person.employeeNo;
    in >> person.department;
    in >> person.post;
    in >> person.salary;
    return in;
}

std::ostream& operator<<(std::ostream& out, Person& person) {
    out << person.id << std::endl;
    out << person.name << std::endl;
    out << person.sex << std::endl;
    out << person.birthday << std::endl;
    out << person.address << std::endl;
    out << person.phoneNo << std::endl;
    out << person.employeeNo << std::endl;
    out << person.department << std::endl;
    out << person.post << std::endl;
    out << person.salary;
    return out;
}
