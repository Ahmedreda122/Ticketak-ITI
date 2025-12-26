#include "User.cpp"

class Admin : public User {
public:
    Admin(string name, string email, string password, char gender, string phoneNumber): 
    User(name, email, password, gender, phoneNumber){}
    // Note: The diagram links this to EventManager, 
    // but implies Admin might manage instances directly.
    bool createEvent(); 
    bool addEvent(int eventId);
    bool deleteEvent(int eventId);
};