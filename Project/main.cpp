#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory> // For smart pointers (optional, but good practice)
#include <algorithm>
#include <GenericMultiEditorForm.cpp>
#include <EventManager.cpp>

using namespace std;

// ================= ENUMS =================
enum class UserType {
    Fan = 1
    Admin = 2,
    NotAuth = 0,
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

// DTO for Login Data
struct LoginDTO
{
    string email;
    string password;
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
protected: 
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
    // Note: The diagram links this to EventManager, 
    // but implies Admin might manage instances directly.
    bool createEvent(); 
    bool addEvent(int eventId);
    bool deleteEvent(int eventId);
};

// ================= SINGLETON VECTORS (DATABASES) =================

// // Singleton Class for Fans
// class FanManager {
// private:
//     vector<Fan> fans;

//     // Private constructor
//     FanManager() = default;

//     // Disable copy & assignment
//     FanManager(const FanManager&) = delete;
//     FanManager& operator=(const FanManager&) = delete;

// public:
//     static FanManager& getInstance() {
//         static FanManager instance; // Magic Static
//         return instance;
//     }

//     void addFan(const Fan& e) {
//         fans.push_back(e);
//     }

//     const vector<Fan>& getFans() const {
//         return fans;
//     }

//     Fan* getFan(int ID) {
//         auto it = find_if(fans.begin(), fans.end(),
//             [ID](Fan& e) {
//                 return e.getId() == ID;
//             }
//         );

//         if (it != fans.end())
//             return &(*it);

//         return nullptr;
//     }
// };

// Singleton Class for Admins
// class AdminManager {
// private:
//     vector<Admin> admins;

//     // Private constructor
//     AdminManager() = default;

//     // Disable copy & assignment
//     AdminManager(const AdminManager&) = delete;
//     AdminManager& operator=(const AdminManager&) = delete;

// public:
//     static AdminManager& getInstance() {
//         static AdminManager instance; // Magic Static
//         return instance;
//     }

//     void addAdmin(const Admin& e) {
//         admins.push_back(e);
//     }

//     const vector<Admin>& getAdmins() const {
//         return admins;
//     }

//     Admin* getAdmin(string email) {
//         auto it = find_if(admins.begin(), admins.end(),
//             [email](Admin& admin) {
//                 return admin.getEmail() == email;
//             }
//         );

//         if (it != admins.end())
//             return &(*it);

//         return nullptr;
//     }
// };


// Singleton Class for Events
// class EventManager {
// private:
//     vector<Event> events;

//     // Private constructor
//     EventManager() = default;

//     // Disable copy & assignment
//     EventManager(const EventManager&) = delete;
//     EventManager& operator=(const EventManager&) = delete;

// public:
//     static EventManager& getInstance() {
//         static EventManager instance; // Magic Static
//         return instance;
//     }

//     void addEvent(const Event& e) {
//         events.push_back(e);
//     }

//     const vector<Event>& getEvents() const {
//         return events;
//     }

//     Event* getEvent(int ID) {
//         auto it = find_if(events.begin(), events.end(),
//             [ID](Event& e) {
//                 return e.getId() == ID;
//             }
//         );

//         if (it != events.end())
//             return &(*it);

//         return nullptr;
//     }
// };

// ================= SERVICES =================

static class AuthenticationService {
public:
    // Logic uses FanManager and AdminManager to verify credentials
    // Returns Pointer to the Fan/Admin or nullptr for wrong crenditials
    static User* login(const LoginDTO& user, UserType userType) {
        if (userType == UserType::Fan) {
            FanManager fanManager = FanManager::getInstance();
            return fanManager.getFanByEmailPass(user.email, user.password);
        } else if (userType == UserType::Admin){
            AdminManager adminManager = AdminManager::getInstance();
            return adminManager.getAdminByEmailPass(user.email, user.password);
        }
        return nullptr;
    }
    static bool registerUser(User user, UserType type); 
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
    Admin* currentAdmin = nullptr;
    Fan* currentFan = nullptr;
    UserType userType = UserType::NotAuth;
public:
    void run(); // Main loop

    // Make current Admin Points at an Admin returned from Login Method (AdminManager.admins vector)
    void setCurrentAdmin(Admin& admin) {
        currentAdmin = &admin;
        currentFan = nullptr;
        userType = UserType::Admin;
    }

    // Make current Fan Points at a Fan returned from Login Method (FanManager.Fans vector)
    void setCurrentFan(Fan& fan) {
        currentFan = &fan;
        currentAdmin = nullptr;
        userType = UserType::Fan;
    }

    // Note: SystemManager does not Own these Objects (FanManager & AdminManager Do)
    // so we don't delete them only points to them or points to null
    void logout() {
        currentAdmin = nullptr;
        currentFan = nullptr;
        userType = UserType::NotAuth;
    }
    
    bool isAdmin(){
        return userType == UserType::Admin;
    }

    bool isFan() {
        return userType == UserType::Fan;
    }
    
    void viewEventsPage();
    int viewAdminMenu();
    int viewFanMenu();
    
    int viewLoginForm() {
        vector<Field> loginFormFields = {
            {
                "Email:", 50, 33, 126,
                [](void* user, const char* emailInput) {
                    ((LoginDTO*) user)->email = emailInput;
                }
            },
            { 
                "Password:",50, 32, 126,
                [](void* user, const char* pass) {
                    ((LoginDTO*) user)->password = pass;
                }
            };
        };

        bool abortLoginFormFill = false;
        do {
            // Choosing User Type Menu
            int usertype = displayMenu(vector<string>{"1-Fan\n","2-Admin\n"}, "You want sign in as");
            // if user Press on ESC to back to main menu
            if (userType == -1) {return userType;}

            User* currentUser = nullptr;
            
            do {
                LoginDTO user;
                if !(showForm(&user, loginFormFields)){
                    // if user press on ESC to return to Choosing User Type Menu
                    abortLoginFormFill = true;
                    break;
                }

                currentUser = AuthenticationService::login(user, userType);
                if (user != nullptr){
                    if (userType == UserType::Fan) {
                        Fan* fan = static_cast<Fan*>(currentUser);
                        setCurrentFan(fan);
                        return 1;
                    } else if (userType == UserType::Admin) { 
                        Admin* admin = static_cast<Admin*>(currentUser);
                        setCurrentAdmin(admin);
                        return 2;
                    }
                } 
            } while (currentUser == nullptr);
            
        } while (abortLogin)
    }
    User viewRegisterForm();
    
    void searchEventsByCategory(Category category);
    bool purchasePage(Ticket myTicket);
};


void SystemManager::run() {
    vector<string> menu = {"1- Login\n", "2- Register\n"}

    while(true) {
        int choice = displayMenu(menu, "====================Welcome to Ticketak======================");
    
        switch(choice){
            // Login
            case 1:{
                int result = viewLoginForm();
                
                switch (result)
                {
                // Returning to main menu
                case -1:
                    break;
                // Fan Logs in    
                case 1:
                    break;
                // Admin Logs in    
                case 2:
                    break;
                }
                break;
            }
            case 2:
                break;
            case -1:
                cout << "Thanks for using Ticketak :)\n";
                return;    
        }
    }
}


// ================= MAIN (ENTRY POINT) =================

int main() {
    SystemManager app;
    app.run();
    return 0;
}