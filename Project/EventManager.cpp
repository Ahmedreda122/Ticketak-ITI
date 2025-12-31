#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Event.cpp"

// Singleton Class for Events
class EventManager {
private:
    vector<Event> events;

    // Private constructor
    EventManager() = default;

    // Disable copy & assignment
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

public:
    static EventManager& getInstance() {
        static EventManager instance; // Magic Static
        return instance;
    }

    void addEvent(Event& e) {
        if (e.getId() == 0)
            e.setId(events.size() + 1);
        events.push_back(e);
    }

    bool deleteEvent(int eventId){
        auto it = remove_if(events.begin(), events.end(),
            [eventId](Event& e){ return e.getId() == eventId; });
        if(it != events.end()){
            events.erase(it, events.end());
            return true;
        }
        return false;
    }

    vector<Event>& getEvents() { return events; }
    const vector<Event>& getEvents() const { return events; }

    const int getNEvents() const { return events.size(); }

    Event* getEvent(int ID) {
        auto it = find_if(events.begin(), events.end(),
            [ID](Event& e) {
                return e.getId() == ID;
            }
        );

        if (it != events.end())
            return &(*it);

        return nullptr;
    }
};
