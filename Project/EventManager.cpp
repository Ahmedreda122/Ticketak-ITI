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

    void addEvent(const Event& e) {
        events.push_back(e);
    }

    const vector<Event>& getEvents() const {
        return events;
    }

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
