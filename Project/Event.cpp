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
    Event() = default;

    Event(int id, string name, Category category, Date date,
          TicketTypePriceQuantity vipTickets, TicketTypePriceQuantity economicTickets,
          TicketTypePriceQuantity regularTickets) {
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

    int isValid(string &error) {
        int errC = 0;
        if (name.empty()) {
            error = "Event name cannot be empty";
            errC++;
        }

        if (date.day < 1 || date.day > 31) {
            if (!error.empty()) error += '\n';
            error += "Invalid day";
            errC++;
        }

        if (date.month < 1 || date.month > 12) {
            if (!error.empty()) error += '\n';
            error += "Invalid month";
            errC++;
        }

        if (date.year < 2025) {
            if (!error.empty()) error += '\n';
            error += "Invalid year";
            errC++;
        }

        if (vipTickets.quantity < 0) {
            if (!error.empty()) error += '\n';
            error += "VIP tickets quantity must be greater than or equal to zero";
            errC++;
        }

        if (vipTickets.quantity > 0 && vipTickets.price <= 0) {
            if (!error.empty()) error += '\n';
            error += "VIP tickets price must be greater than zero";
            errC++;
        }

        if (regularTickets.quantity < 0) {
            if (!error.empty()) error += '\n';
            error += "Regular tickets quantity must be greater than or equal to zero";
            errC++;

        }

        if (regularTickets.quantity > 0 && regularTickets.price <= 0) {
            if (!error.empty()) error += '\n';
            error += "Regular tickets price must be greater than zero";
            errC++;
        }

        if (economicTickets.quantity < 0) {
            if (!error.empty()) error += '\n';
            error += " Economic tickets quantity must be greater than or equal to zero";
            errC++;

        }
        if (economicTickets.quantity > 0 && economicTickets.price <= 0) {
            if (!error.empty()) error += '\n';
            error += "Economic tickets price must be greater than zero";
            errC++;
        }

        return errC;
    }

    const TicketTypePriceQuantity &getVipTickets() const {
        return vipTickets;
    }

    const TicketTypePriceQuantity &getEconomicTickets() const {
        return economicTickets;
    }

    const TicketTypePriceQuantity &getRegularTickets() const {
        return regularTickets;
    }

    int getId() const { return id; }

    string getName() const { return name; }

    Category getCategory() const {
        return category;
    }

    int getDay() const {
        return date.day;
    }

    int getMonth() const {
        return date.month;
    }

    int getYear() const {
        return date.year;
    }

    string getTicketPriceStr(TicketType type) const {
        switch (type) {
            case TicketType::VIP:
                return to_string(vipTickets.price);
            case TicketType::Regular:
                return to_string(regularTickets.price);
            case TicketType::Economic:
                return to_string(economicTickets.price);
            default:
                return "0";
        }
    }

    string getTicketQuantityStr(TicketType type) const {
        switch (type) {
            case TicketType::VIP:
                return to_string(vipTickets.quantity);
            case TicketType::Regular:
                return to_string(regularTickets.quantity);
            case TicketType::Economic:
                return to_string(economicTickets.quantity);
            default:
                return "0";
        }
    }

    void setDay(int day) {
        this->date.day = day;
    }

    void setMonth(int month) {
        this->date.month = month;
    }

    void setYear(int year) {
        this->date.year = year;
    }

    void setTicketPrice(TicketType type, double price) {
        switch (type) {
            case TicketType::VIP:
                vipTickets.price = price;
                break;
            case TicketType::Regular:
                regularTickets.price = price;
                break;
            case TicketType::Economic:
                economicTickets.price = price;
                break;
        }
    }

    void setTicketQuantity(TicketType type, int quantity) {
        switch (type) {
            case TicketType::VIP:
                vipTickets.quantity = quantity;
                break;
            case TicketType::Regular:
                regularTickets.quantity = quantity;
                break;
            case TicketType::Economic:
                economicTickets.quantity = quantity;
                break;
        }

        // keep totals consistent
        capacity = vipTickets.quantity +
                   regularTickets.quantity +
                   economicTickets.quantity;

        availableTickets = capacity;
    }

    void setId(int id) {
        this->id = id;
    }

    void setName(const string &name) {
        this->name = name;
    }

    void setCategory(Category category) {
        this->category = category;
    }

    string dateToString(Date date) const {
        string d = (date.day < 10 ? "0" : "") + to_string(date.day);
        string m = (date.month < 10 ? "0" : "") + to_string(date.month);
        return d + "-" + m + "-" + to_string(date.year);
    }

    string categoryToString(Category category) const {
        switch (category) {
            case Category::Sports:
                return "Sports";
            case Category::Parties:
                return "Parties";
            case Category::Carnivals:
                return "Carnivals";
            default:
                return "Unknown";
        }
    }

    // Logic to link fan to ticket
    // Note if returned ticket has id="0" , then booking operation is failed
    Ticket bookEvent(int fanId, TicketTypePrice typePrice) {
        Ticket createdTicket;
        switch (typePrice.type) {
            case TicketType::VIP:
                if (vipTickets.quantity <= 0) {
                    return Ticket();
                }
                vipTickets.quantity--;
                break;
            case TicketType::Economic:
                if (economicTickets.quantity <= 0) {
                    return Ticket();
                }
                economicTickets.quantity--;
                break;
            case TicketType::Regular:
                if (regularTickets.quantity <= 0) {
                    return Ticket();
                }
                regularTickets.quantity--;
                break;
            default:
                return Ticket();
                break;
        }
        availableTickets--;

        createdTicket.setId(to_string(tickets.size() + 1));
        createdTicket.setFanId(fanId);
        createdTicket.setEventId(id);
        createdTicket.setTicketTypePrice(typePrice);
        createdTicket.setTicketStatus(TicketStatus::Reserved);

        tickets.push_back(createdTicket);
        return createdTicket;
    }

    string viewDetailsBreifly() {
        return "Name: " + name + " , Category: " + categoryToString(category) + " , Date: " + dateToString(date);
    }

    string viewDetails() const {
        return "  Event #" + to_string(id) + "\n  Name: " + name + "\n  Category: " + categoryToString(category) +
               "\n  Date: " + dateToString(date) + "\n  Total Seats: " + to_string(capacity) + "\n  Available Seats: " +
               to_string(availableTickets)
               + "\n  VIP Ticket Price: " + to_string(vipTickets.price) + " , VIP Available Tickets: " +
               to_string(vipTickets.quantity)
               + "\n  Regular Ticket Price: " + to_string(regularTickets.price) + " , Regular Available Tickets: " +
               to_string(regularTickets.quantity)
               + "\n  Economic Ticket Price: " + to_string(economicTickets.price) + " , Economic Available Tickets: " +
               to_string(economicTickets.quantity);
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

    void expireTickets() {
        for (int i = 0; i < tickets.size(); i++) {
            tickets[i].changeStatus(TicketStatus::Expired);
        }
    }

    // Helper to populate tickets
    void addTicket(Ticket t) { tickets.push_back(t); }
};
