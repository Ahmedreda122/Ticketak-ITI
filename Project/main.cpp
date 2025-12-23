#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory> // For smart pointers (optional, but good practice)

using namespace std;

// ================= ENUMS =================
enum class UserType {
    Admin,
    Fan
};

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

enum class Category {
    Sports,
    Parties,
    Carnivals
};

// ================= FORWARD DECLARATIONS =================
class Event;
class Ticket;
class Fan;
class Admin;

// ================= STRUCTS / HELPERS =================
// Represents the <<Map>> TicketTypePrice
struct TicketTypePrice {
    TicketType type;
    double price;
};

// Placeholder for Date since C++ doesn't have a primitive Date type
struct Date {
    int day, month, year;
};

// ================= CORE CLASSES =================

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
    void viewDetails();
    string getEventStatus();
    void expireTickets();

    int getId() const { return id; }
    // Helper to populate tickets
    void addTicket(Ticket t) { tickets.push_back(t); }
};

// ================= USER HIERARCHY =================

class User {
protected: // '#' in UML means protected
    string email;
    string password;
    char gender;
    string phoneNumber;

public:
    virtual ~User() = default; // Virtual destructor for base class
    
    string getEmail() const { return email; }
    string getPassword() const { return password; }
};

class Fan : public User {
private:
    int id;
    vector<Ticket> myTickets;

public:
    void buyTicket(Ticket myTicket);
    vector<Ticket> viewMyTickets();
    int getId() const { return id; }
};

class Admin : public User {
public:
    // Note: The diagram links this to EventsVec, 
    // but implies Admin might manage instances directly.
    bool createEvent(); 
    bool addEvent(int eventId);
    bool deleteEvent(int eventId);
};

// ================= SINGLETON VECTORS (DATABASES) =================

// Singleton Class for Fans
class FanVec {
private:
    static FanVec* instance;
    vector<Fan> fans;
    FanVec() {} // Private Constructor

public:
    static FanVec& getInstance(); // Returns reference to singleton
    void addFan(Fan fan);
    const vector<Fan>& getFans() const;
    Fan& getFan(int id);
    int getNFan(); // Count
};

// Singleton Class for Admins
class AdminVec {
private:
    static AdminVec* instance;
    vector<Admin> admins;
    AdminVec() {}

public:
    static AdminVec& getInstance();
    void addAdmin(Admin admin);
    const vector<Admin>& getAdmins() const;
    Admin& getAdmin(int id);
    int getNAdmin();
};

// Singleton Class for Events
class EventsVec {
private:
    static EventsVec* instance;
    vector<Event> events;
    EventsVec() {}

public:
    static EventsVec& getInstance();
    void addEvent(Event event);
    const vector<Event>& getEvents() const;
    Event& getEvent(int id);
    int getNEvents();
};

// ================= SERVICES =================

class AuthenticationService {
public:
    // Logic uses FanVec and AdminVec to verify credentials
    bool login(string email, string password, UserType userType);
    bool registerUser(User user, UserType type); 
};

// ================= PAYMENT STRATEGY PATTERN =================

// Abstract Interface
class PaymentMethod {
public:
    virtual bool pay(double amount) = 0; // Pure virtual function
    virtual ~PaymentMethod() = default;
};

class FawryPay : public PaymentMethod {
public:
    // Stateless payment logic
    bool pay(double amount) override {
        cout << "Paying " << amount << " via Fawry.\n";
        return true;
    }
};

class CreditCard : public PaymentMethod {
private:
    string name;
    string cardNumber;
    string cvv;
    string expiryDate;

public:
    CreditCard(string n, string num, string c, string exp)
        : name(n), cardNumber(num), cvv(c), expiryDate(exp) {}

    bool pay(double amount) override {
        cout << "Paying " << amount << " via CreditCard " << cardNumber << ".\n";
        return true;
    }
};

class PaymentService {
private:
    PaymentMethod* paymentMethod; // Strategy pointer

public:
    void setPaymentMethod(PaymentMethod* method) {
        this->paymentMethod = method;
    }

    bool processPayment(double amount) {
        if (paymentMethod) {
            return paymentMethod->pay(amount);
        }
        return false;
    }
};

// ================= SYSTEM MANAGER (FACADE) =================

class SystemManager {
private:
    Fan* currentFan;
    Admin* currentAdmin;
    UserType userType;

    // Dependencies
    AuthenticationService authService;
    PaymentService paymentService;

public:
    void run(); // Main loop
    void setCurrentAdmin(Admin* admin);
    void setCurrentFan(Fan* fan);
    
    bool isAdmin();
    bool isFan();
    
    void viewEventsPage();
    int viewAdminMenu();
    int viewFanMenu();
    
    string viewLoginForm(); // Returns email/pass string? Or handles UI.
    User viewRegisterForm();
    
    void searchEventsByCategory(Category category);
    bool purchasePage(Ticket myTicket);
    bool logout();
};

// ================= STATIC INITIALIZATION =================
// Singletons need their static members initialized in .cpp usually
FanVec* FanVec::instance = nullptr;
AdminVec* AdminVec::instance = nullptr;
EventsVec* EventsVec::instance = nullptr;

// ================= MAIN (ENTRY POINT) =================

int main() {
    SystemManager app;
    app.run();
    return 0;
}