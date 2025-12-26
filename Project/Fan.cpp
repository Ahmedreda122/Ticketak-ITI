#include "User.cpp"
#include "Ticket.cpp"
#include <vector>

class Fan : public User {
private:
    int id;
    vector<Ticket> myTickets;

public:
    void buyTicket(Ticket myTicket);
    vector<Ticket> viewMyTickets();
    void setId(int _id)
    {
        id = _id;
    }
    int getId() const { return id; }
};
