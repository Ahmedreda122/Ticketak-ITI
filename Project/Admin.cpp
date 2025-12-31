#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "User.cpp"
#include "EventManager.cpp"

using namespace std;

class Admin : public User {
private:
public:
    Admin() = default;
    Admin(string name, string email, string password, char gender, string phone)
        : User(name,email,password,gender,phone) {}
};
