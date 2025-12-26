#pragma once

#include<string>
using namespace std;

class User {
protected: 
    string email;
    string password;
    char gender;
    string phoneNumber;

public:
    virtual ~User() = default; // Virtual destructor for base class
    
    string getEmail() const { return email; }
    string getPassword() const { return password; }
};