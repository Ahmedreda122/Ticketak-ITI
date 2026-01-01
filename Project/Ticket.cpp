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
    Ticket() : Ticket("0", 0, 0, TicketTypePrice{TicketType::Economic, 0}) {}

    Ticket(string id, int eventId, int fanId,TicketTypePrice typePrice){
        this->id = id;
        this-> eventId = eventId;
        this->fanId = fanId;
        this->typePrice = typePrice;
        status = TicketStatus::Available;
    }

    string getId() {return id; }
    int getEventId() {return eventId; }
    int getFanId() {return fanId; }
    double getPrice() { return typePrice.price; }
    TicketStatus getTicketStatus() const { return status; }
    
    string getTicketStatusStr() const { 
        switch(status) {
            case (TicketStatus::Available):
                return "Available";
            case (TicketStatus::Reserved):
                return "Reserved";
            case (TicketStatus::Expired):
                return "Expired";
            default: 
                return "Unknown";            
        }
    }

    TicketTypePrice getTypePrice() { return typePrice;}

    string getType(){
        switch (typePrice.type) {
            case TicketType::VIP:      return "VIP";
            case TicketType::Economic: return "Economic";
            case TicketType::Regular:  return "Regular";
            default:                   return "Unknown";
        }
    }

    // Getters and Setters
    void setId(string id) { this->id = id; }
    void setEventId(int eventId) { this->eventId = eventId; }
    void setFanId(int id) { fanId = id; }
    void setTicketTypePrice(TicketTypePrice typePrice) { this->typePrice = typePrice; }
    void setTicketStatus(TicketStatus status) { this->status = status; }
};
