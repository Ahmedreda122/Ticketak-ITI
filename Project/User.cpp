#pragma once

#include <string>
using namespace std;

class User {
protected:
    string email;
    string password;
    char gender;
    string phoneNumber;

public:
    virtual ~User() = default; // Virtual destructor for base class

    // Getters
    string getEmail() const { return email; }
    string getPassword() const { return password; }
    char getGender() const { return gender; }
    string getPhoneNumber() const { return phoneNumber; }

    // Setters
    void setEmail(const string& email) { this->email = email; }
    void setPassword(const string& password) { this->password = password; }
    void setGender(char gender) { this->gender = gender; }
    void setPhoneNumber(const string& phoneNumber) {
        this->phoneNumber = phoneNumber;
    }
};
