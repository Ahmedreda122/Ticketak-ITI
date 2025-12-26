#pragma once

#include <string>
using namespace std;

class User {
protected:
    string name;
    string email;
    string password;
    char gender;
    string phoneNumber;

public:
    virtual ~User() = default; // Virtual destructor for base class
    User() = default;
    User(string name, string email, string password, char gender, string phoneNumber): 
    name(name), email(email), password(password), gender(gender), phoneNumber(phoneNumber) {}
    // Getters
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }
    char getGender() const { return gender; }
    string getPhoneNumber() const { return phoneNumber; }

    // Setters
    void setName(const string& name) { this->name = name; }
    void setEmail(const string& email) { this->email = email; }
    void setPassword(const string& password) { this->password = password; }
    void setGender(char gender) { this->gender = gender; }
    void setPhoneNumber(const string& phoneNumber) {
        this->phoneNumber = phoneNumber;
    }
};
