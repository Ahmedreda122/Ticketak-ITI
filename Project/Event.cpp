#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "Ticket.cpp"

using namespace std;

enum class Category {
    Sports = 1,
    Parties = 2,
    Carnivals = 3,
    Other = 4
};

struct TicketTypePriceQuantity {
    TicketType type;
    double price;
    int quantity;
};

struct Date {
    int day, month, year;
};

enum class EventStatus {
    Upcoming = 1,
    Finished = 2
};

class Event {
private:
    int id = 0;
    string name;
    Category category = Category::Other;
    int capacity;
    int availableTickets;

    vector<Ticket> tickets; // Composition: Event contains Tickets
    TicketTypePriceQuantity vipTickets;
    TicketTypePriceQuantity economicTickets;
    TicketTypePriceQuantity regularTickets;
    Date date;

    bool isPastDate(const Date& eventDate) const {
        time_t t = time(nullptr);
        tm today{};

        localtime_s(&today, &t); // Windows
        // localtime_r(&t, &today); // Linux / macOS

        // Compare year
        if (eventDate.year < today.tm_year + 1900) return true;
        if (eventDate.year > today.tm_year + 1900) return false;

        // Compare month
        if (eventDate.month < today.tm_mon + 1) return true;
        if (eventDate.month > today.tm_mon + 1) return false;

        // Compare day
        return eventDate.day < today.tm_mday;
    }

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
            case Category::Other:
                return "Other";    
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

    string viewDetailsBreifly() const {
        return "Event #"+to_string(id)+", Name: "+name+", Category: "+categoryToString(category)+", Date: "+dateToString(date);
    }

    string viewDetails() const {
        return "  Event #" + to_string(id) + "\n  Name: " + name + "\n  Category: " + categoryToString(category) +
            "\n  Date: " + dateToString(date) + "\n  Total Seats: " + to_string(capacity) + "\n  Available Seats: " +
            to_string(availableTickets) + "\n  Event Status: " + eventStatustoStr(getEventStatus()) 
            + "\n  VIP Ticket Price: " + to_string(vipTickets.price) + " , VIP Available Tickets: " +
            to_string(vipTickets.quantity)
            + "\n  Regular Ticket Price: " + to_string(regularTickets.price) + " , Regular Available Tickets: " +
            to_string(regularTickets.quantity)
            + "\n  Economic Ticket Price: " + to_string(economicTickets.price) + " , Economic Available Tickets: " +
            to_string(economicTickets.quantity);
    }

    EventStatus getEventStatus() const {
        return isPastDate(date)
            ? EventStatus::Finished
            : EventStatus::Upcoming;
    }

    string eventStatustoStr(const EventStatus& eventStatus) const {
        switch (eventStatus) {
            case EventStatus::Upcoming:
                return "Upcoming";
            case EventStatus::Finished:
                return "Finished";
            default:
                return "Unknown";
        }
    }

    void expireTickets() {
        for (int i = 0; i < tickets.size(); i++) {
            tickets[i].setTicketStatus(TicketStatus::Expired);
        }
    }

    // Helper to populate tickets
    void addTicket(Ticket t) { tickets.push_back(t); }
};
