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
    void changeStatus(TicketStatus status);
    double getPrice();
    string getType();
    
    // Getters and Setters
    void setFanId(int id) { fanId = id; }
};