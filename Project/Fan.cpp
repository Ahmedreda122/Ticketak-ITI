#pragma once

#include "User.cpp"
#include "Ticket.cpp"
#include <vector>
#include <string>

using namespace std;

class Fan : public User {
private:
    int id = 0;
    vector<Ticket> myTickets;

public:
    Fan() = default;

    Fan(string name, string email, string password, char gender, string phone)
        : User(name,email,password,gender,phone), id(0) {}

    void buyTicket(Ticket myTicket)
    {
        myTickets.push_back(myTicket);
    }

    vector<Ticket> getMyTickets() const
    {
        return myTickets;
    }

    vector<string> buildTicketsMenuItems()
    {
        vector<string> ticketItems;

        if (myTickets.empty()) {
            ticketItems.push_back("No tickets available.\n");
            return ticketItems;
        }

        auto& eventManager = EventManager::getInstance();
        // Create a menu item for each ticket
        for (size_t i = 0; i < myTickets.size(); i++) {
            Ticket& currTicket = myTickets[i];

            // Update Ticket Status if Needed
            if (currTicket.getTicketStatus() != TicketStatus::Expired){
                Event* e = eventManager.getEvent(currTicket.getEventId());
                if (e && e->getEventStatus() == EventStatus::Finished) { e->expireTickets(); }   
            }

            string item =
                to_string(i + 1) + "- Ticket ID: " + currTicket.getId() +
                " | Type: " + currTicket.getType() +
                " | Price: " + to_string(currTicket.getPrice()) +
                " | Status: " + currTicket.getTicketStatusStr() +
                "\n";

            ticketItems.push_back(item);
        }

        return ticketItems;
    }

    string getTicketDetails(int index)
    {
        if (index < 0 || index >= (int)myTickets.size()) {
            return "Invalid ticket index.";
        }

        Ticket& t = myTickets[index];
        string details = "========== Ticket Details ==========\n";
        details += "Ticket ID: " + t.getId() + "\n";
        details += "Event ID: " + to_string(t.getEventId()) + "\n";
        details += "Type: " + t.getType() + "\n";
        details += "Price: " + to_string(t.getPrice()) + " EGP\n";
        details += "Status: " + t.getTicketStatusStr() + "\n";
        details += "===================================\n";

        return details;
    }

    void setId(int _id)
    {
        id = _id;
    }

    int getId() const { return id; }
};
