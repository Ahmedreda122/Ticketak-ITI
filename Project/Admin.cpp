#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "User.cpp"
#include "EventManager.cpp"

using namespace std;

class Admin : public User {
private:
    int id;

public:
    Admin() = default;
    Admin(int _id, string name, string email, string password, char gender, string phone)
        : User(name,email,password,gender,phone), id(_id) {}
    Admin(string name, string email, string password, char gender, string phone)
        : User(name,email,password,gender,phone), id(0) {}

    void setId(int _id) { id = _id; }
    int getId() const { return id; }

    bool createEvent(const Event& e){
        EventManager::getInstance().addEvent(e);
        return true;
    }

    bool deleteEvent(int eventId){
        auto& events = EventManager::getInstance().getEvents();
        auto it = remove_if(events.begin(), events.end(),
            [eventId](Event& e){ return e.getId() == eventId; });
        if(it != events.end()){
            events.erase(it, events.end());
            return true;
        }
        return false;
    }

    void viewAllEvents(){
        const auto& events = EventManager::getInstance().getEvents();
        if(events.empty()){
            cout << "No events available.\n";
            return;
        }
        for(const auto& e : events){
            e.viewDetails();
            cout << "----------------\n";
        }
    }
};
