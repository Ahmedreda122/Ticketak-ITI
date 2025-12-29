#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Ticket.cpp"

using namespace std;

enum class Category {
    Sports,
    Parties,
    Carnivals
};
struct TicketTypePriceQuantity {
     TicketType type;
     double price;
     int quantity;
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
    // tickets vector should be removed as we will store each ticket type and number of tickets available of it instead
    vector<Ticket> tickets; // Composition: Event contains Tickets
    TicketTypePriceQuantity vipTickets;
    TicketTypePriceQuantity economicTickets;
    TicketTypePriceQuantity regularTickets;
    Date date;

public:
    Event(int id,string name,Category category,Date date,
          TicketTypePriceQuantity vipTickets,TicketTypePriceQuantity economicTickets,TicketTypePriceQuantity regularTickets)
    {
        this->id = id;
        this->name = name;
        this->category = category;
        this->date = date;
        this->vipTickets = vipTickets;
        this->economicTickets = economicTickets;
        this->regularTickets = regularTickets;
        this->capacity = vipTickets.quantity + economicTickets.quantity + regularTickets.quantity;
        availableTickets = capacity;
    }
    const TicketTypePriceQuantity& getVipTickets() const {
        return vipTickets;
    }

    const TicketTypePriceQuantity& getEconomicTickets() const {
        return economicTickets;
    }

    const TicketTypePriceQuantity& getRegularTickets() const {
        return regularTickets;
    }

    string dateToString(Date date) const{
        string d = (date.day < 10 ? "0" : "") + to_string(date.day);
        string m = (date.month < 10 ? "0" : "") + to_string(date.month);
        return d + "-" + m + "-" + to_string(date.year);
    }
    string categoryToString(Category category) const{
        switch(category) {
            case Category::Sports: return "Sports";
            case Category::Parties: return "Parties";
            case Category::Carnivals: return "Carnivals";
            default: return "Unknown";
        }
    }
    // Logic to link fan to ticket
    // Note if returned ticket has id="0" , then booking operation is failed
    Ticket bookEvent(int fanId, TicketTypePrice typePrice)
    {
        Ticket createdTicket;
        switch (typePrice.type) {
            case TicketType::VIP:
                if(vipTickets.quantity <= 0)
                {
                    return Ticket();
                }
                vipTickets.quantity--;
                break;
            case TicketType::Economic:
                if(economicTickets.quantity <= 0)
                {
                    return Ticket();
                }
                economicTickets.quantity--;
                break;
            case TicketType::Regular:
                if(regularTickets.quantity <= 0)
                {
                    return Ticket();
                }
                regularTickets.quantity--;
                break;
            default:
                return Ticket();
                break;
        }
        availableTickets--;

        createdTicket.setId(to_string(tickets.size()+1));
        createdTicket.setFanId(fanId);
        createdTicket.setEventId(id);
        createdTicket.setTicketTypePrice(typePrice);
        createdTicket.setTicketStatus(TicketStatus::Reserved);

        tickets.push_back(createdTicket);
        return createdTicket;
    }
    string viewDetailsBreifly()
    {
        return "Name: "+name+" , Category: "+categoryToString(category)+" , Date: "+dateToString(date);
    }
    string viewDetails() const
    {
        return "  Event #" + to_string(id) + "\n  Name: " + name + "\n  Category: " + categoryToString(category) +
        "\n  Date: " + dateToString(date) + "\n  Total Seats: " + to_string(capacity) + "\n  Available Seats: " + to_string(availableTickets)
        + "\n  VIP Ticket Price: " + to_string(vipTickets.price) + " , VIP Available Tickets: " + to_string(vipTickets.quantity)
        + "\n  Regular Ticket Price: " + to_string(regularTickets.price) + " , Regular Available Tickets: " + to_string(regularTickets.quantity)
        + "\n  Economic Ticket Price: " + to_string(economicTickets.price) + " , Economic Available Tickets: " + to_string(economicTickets.quantity);
        /*
        cout<<"Event #"<<id<<endl;
        cout<<"Name: "<<name<<endl;
        cout<<"Category: "<<categoryToString(category)<<endl;
        cout<<"Date: "<<dateToString(date)<<endl;
        cout<<"Total Seats: "<<capacity<<endl;
        cout<<"Available Seats: "<<availableTickets<<endl;
        cout<<"VIP Ticket Price: "<<vipTickets.price<<" , VIP Available Tickets: "<<vipTickets.quantity<<endl;
        cout<<"Regular Ticket Price: "<<regularTickets.price<<" , Regular Available Tickets: "<<regularTickets.quantity<<endl;
        cout<<"Economic Ticket Price: "<<economicTickets.price<<" , Economic Available Tickets: "<<economicTickets.quantity<<endl;
        */
    }
    string getEventStatus();
    void expireTickets()
    {
        for(int i=0;i<tickets.size();i++)
        {
            tickets[i].changeStatus(TicketStatus::Expired);
        }
    }

    int getId() const { return id; }
    Category getCategory() const {
        return category;
    }
    // Helper to populate tickets
    void addTicket(Ticket t) { tickets.push_back(t); }
};
