#pragma once

#include <string>
#include <vector>

#include "Ticket.cpp"

using namespace std;

enum class Category {
    Sports,
    Parties,
    Carnivals
};


struct Date {
    int day, month, year;
};

class Event {
private:
    int id;
    string name;
    Category category;
    int capacity;
    int availableTickets;
    vector<Ticket> tickets; // Composition: Event contains Tickets
    Date date;

public:
    bool bookEvent(int fanId, int ticketId); // Logic to link fan to ticket
    void viewDetails() const;
    string getEventStatus();
    void expireTickets();

    int getId() const { return id; }
    Category getCategory() const {
        return category;
    }
    // Helper to populate tickets
    void addTicket(Ticket t) { tickets.push_back(t); }
};


