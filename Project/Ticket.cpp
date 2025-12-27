#pragma once

#include <string>

using namespace std;

enum class TicketType {
    VIP,
    Economic,
    Regular
};

enum class TicketStatus {
    Available,
    Reserved,
    Expired
};

struct TicketTypePrice {
    TicketType type;
    double price;
};

class Ticket {
private:
    string id;
    int eventId;
    int fanId;
    TicketTypePrice typePrice;
    TicketStatus status;

public:
    Ticket(string id, int eventId, int fanId,TicketTypePrice typePrice)
    {
        this->id = id;
        this-> eventId = eventId;
        this->fanId = fanId;
        this->typePrice = typePrice;
        status = TicketStatus::Available;
    }
    void changeStatus(TicketStatus status);
    string getId() {return id; }
    int getEventId() {return eventId; }
    int getFanId() {return fanId; }
    double getPrice() { return typePrice.price; }
    TicketTypePrice getTypePrice() { return typePrice;}
    string getType()
    {
        switch (typePrice.type) {
            case TicketType::VIP:      return "VIP";
            case TicketType::Economic: return "Economic";
            case TicketType::Regular:  return "Regular";
            default:                   return "Unknown";
        }
    }

    // Getters and Setters
    void setFanId(int id) { fanId = id; }
};
