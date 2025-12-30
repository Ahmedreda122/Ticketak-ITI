#pragma once

#include "User.cpp"
#include "Ticket.cpp"
#include <vector>
#include <string>

using namespace std;

class Fan : public User {
private:
    int id;
    vector<Ticket> myTickets;

public:
    void buyTicket(Ticket myTicket)
    {
        myTickets.push_back(myTicket);
    }

    vector<Ticket> viewMyTickets()
    {
        return myTickets;
    }

    vector<string> getTicketsMenuItems()
    {
        vector<string> ticketItems;

        if (myTickets.empty()) {
            ticketItems.push_back("No tickets available.\n");
            return ticketItems;
        }

        // Create a menu item for each ticket
        for (size_t i = 0; i < myTickets.size(); i++) {
            string item = to_string(i + 1) + "- Ticket ID: " + myTickets[i].getId()
                        + " | Type: " + myTickets[i].getType()
                        + " | Price: " + to_string(myTickets[i].getPrice()) + "\n";
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
        details += "===================================\n";

        return details;
    }

    void setId(int _id)
    {
        id = _id;
    }

    int getId() const { return id; }
};
