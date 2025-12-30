#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <regex>
#include <string>

#include "GenericMultiEditorForm.cpp"

#include "EventManager.cpp"
#include "FanManager.cpp"
#include "AdminManager.cpp"

using namespace std;

// ================= ENUMS =================
enum class UserType {
    Fan = 1,
    Admin = 2,
    NotAuth = 0
};

enum class EventFieldId {
    Name,
    Category,
    //Capacity,

    DateDay,
    DateMonth,
    DateYear,

    VipPrice,
    VipQuantity,

    RegularPrice,
    RegularQuantity,

    EconomicPrice,
    EconomicQuantity
};


struct EventField : Field {
    EventFieldId id;
};


// DTO for Login Data
struct LoginDTO {
    string email;
    string password;
};

// ================= SERVICES =================
class ValidationService {
public:
    static bool isValidEmail(const string &email) {
        // A Form of E-mail following IETF standards
        regex stdEmail("[^@.]+(.[^@.]+)?(@[A-Za-z0-9]+)(\\-[A-Za-z0-9]+)?(.[0-9]*[A-Za-z]+[0-9]*(\\-[A-Za-z0-9]+)?)+");
        // If the user's email following the regex form return true, false otherwise
        return regex_match(email, stdEmail);
    }

    static bool isValidNum(const string &phoneNum) {
        // Standard form of Egyptian phone number
        regex stdPhoneNum("(010|011|012|015)[0-9]{8}");
        // If the user's phone number following the regex form return true, false otherwise
        return regex_match(phoneNum, stdPhoneNum);
    }

    static bool isValidUserName(const string &userName) {
        // User name should contain 4 to 20 characters
        return (userName.size() <= 20 && userName.size() >= 3);
        // {
        //     // Return false if bigger than 20 or lower than 4
        //     return false;
        // }

        // // A Standard form for username
        // regex stdUserName("([0-9_]*[a-zA-Z]+[_0-9]*[a-zA-Z]+[0-9_]*)");
        // // If the user's username following the regex form return true, false otherwise
        // return regex_match(userName, stdUserName);
    }

    static bool isValidPassword(const string &password) {
        return (password.size() >= 8);
    }
};

class AuthenticationService {
public:
    // Logic uses FanManager and AdminManager to verify credentials
    // Returns Pointer to the Fan/Admin or nullptr for wrong credentials
    static User *login(const LoginDTO &user, UserType userType) {
        if (userType == UserType::Fan) {
            FanManager &fanManager = FanManager::getInstance();
            return fanManager.getFanByEmailPass(user.email, user.password);
        } else if (userType == UserType::Admin) {
            AdminManager &adminManager = AdminManager::getInstance();
            return adminManager.getAdminByEmailPass(user.email, user.password);
        }
        return nullptr;
    }
    
    // Only the Fan can register, Admin Added by developer
    static bool _register(Fan &fan) {
        FanManager &fanManager = FanManager::getInstance();
        
        fan.setId(fanManager.getSize());
        fanManager.addFan(fan);
        return true;
    }

    static bool isExistingEmail(const string& email) {
        FanManager &fanManager = FanManager::getInstance();
        AdminManager &adminManager = AdminManager::getInstance();
        return fanManager.getFanByEmail(email) != nullptr || adminManager.getAdminByEmail(email) != nullptr;
    }    
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

    void setName(const string& n) { name = n; }
    void setCardNumber(const string& num) { cardNumber = num; }
    void setCvv(const string& c) { cvv = c; }
    void setExpiryDate(const string& exp) { expiryDate = exp; }

    bool pay(double amount) override {
        cout << "Paying " << amount << " via CreditCard " << cardNumber << ".\n";
        return true;
    }
};

class PaymentService {
private:
    PaymentMethod *paymentMethod; // Strategy pointer

public:
    void setPaymentMethod(PaymentMethod *method) {
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
    Admin *currentAdmin = nullptr;
    Fan *currentFan = nullptr;
    UserType userType = UserType::NotAuth;
    const string EMAIL_ALLOWED_CHARS = "A-Za-z0-9_@.%+-";

public:
    void run(); // Main loop

    // Make current Admin Points at an Admin returned from Login Method (AdminManager.admins vector)
    void setCurrentAdmin(Admin &admin) {
        currentAdmin = &admin;
        currentFan = nullptr;
        userType = UserType::Admin;
    }

    // Make current Fan Points at a Fan returned from Login Method (FanManager.Fans vector)
    void setCurrentFan(Fan &fan) {
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

    bool isAdmin() {
        return userType == UserType::Admin;
    }

    bool isFan() {
        return userType == UserType::Fan;
    }

    vector<EventField>& createEventFormFields() {
        static vector<EventField> fields = {
            // -------- BASIC INFO --------
            {
                {
                    "Event Name:",
                    30,
                    "A-Za-z ",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setName(v);
                    }
                },
                EventFieldId::Name
            },

            // -------- DATE --------
            {
                {
                    "Day:",
                    2,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setDay(atoi(v));
                    }
                },
                EventFieldId::DateDay
            },
            {
                {
                    "Month:",
                    2,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setMonth(atoi(v));
                    }
                },
                EventFieldId::DateMonth
            },
            {
                {
                    "Year:",
                    4,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setYear(atoi(v));
                    }
                },
                EventFieldId::DateYear
            },

            // -------- VIP TICKETS --------
            {
                {
                    "VIP Ticket Price:",
                    7,
                    "0-9.",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketPrice(TicketType::VIP, atof(v));
                    }
                },
                EventFieldId::VipPrice
            },
            {
                {
                    "VIP Ticket Quantity:",
                    6,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketQuantity(TicketType::VIP, atoi(v));
                    }
                },
                EventFieldId::VipQuantity
            },

            // -------- REGULAR TICKETS --------
            {
                {
                    "Regular Ticket Price:",
                    7,
                    "0-9.",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketPrice(TicketType::Regular, atof(v));
                    }
                },
                EventFieldId::RegularPrice
            },
            {
                {
                    "Regular Ticket Quantity:",
                    6,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketQuantity(TicketType::Regular, atoi(v));
                    }
                },
                EventFieldId::RegularQuantity
            },

            // -------- ECONOMIC TICKETS --------
            {
                {
                    "Economic Ticket Price:",
                    7,
                    "0-9.",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketPrice(TicketType::Economic, atof(v));
                    }
                },
                EventFieldId::EconomicPrice
            },
            {
                {
                    "Economic Ticket Quantity:",
                    6,
                    "0-9",
                    [](void* obj, const char* v) {
                        static_cast<Event*>(obj)->setTicketQuantity(TicketType::Economic, atoi(v));
                    }
                },
                EventFieldId::EconomicQuantity
            }
        };

        return fields;
    }


    void fillEditEventData(vector<EventField>& fields, const Event& event) {
        for (auto& f : fields) {
            switch (f.id) {
                case EventFieldId::Name:
                    f.oldValue = event.getName();
                    break;
                case EventFieldId::DateDay:
                    f.oldValue = to_string(event.getDay());
                    break;

                case EventFieldId::DateMonth:
                    f.oldValue = to_string(event.getMonth());
                    break;

                case EventFieldId::DateYear:
                    f.oldValue = to_string(event.getYear());
                    break;

                case EventFieldId::VipPrice:
                    f.oldValue = event.getTicketPriceStr(TicketType::VIP);
                    break;

                case EventFieldId::VipQuantity:
                    f.oldValue = event.getTicketQuantityStr(TicketType::VIP);
                    break;

                case EventFieldId::RegularPrice:
                    f.oldValue = event.getTicketPriceStr(TicketType::Regular);
                    break;

                case EventFieldId::RegularQuantity:
                    f.oldValue = event.getTicketQuantityStr(TicketType::Regular);
                    break;

                case EventFieldId::EconomicPrice:
                    f.oldValue = event.getTicketPriceStr(TicketType::Economic);
                    break;

                case EventFieldId::EconomicQuantity:
                    f.oldValue = event.getTicketQuantityStr(TicketType::Economic);
                    break;
            }
        }
    }


    bool viewCreateEventForm() {
        vector<EventField>& eventFields = createEventFormFields();
        Event event;
        vector<Field> fields(
            make_move_iterator(eventFields.begin()),
            make_move_iterator(eventFields.end())
        );
        if (showForm(&event, fields, "", 0, 30)) {
            EventManager::getInstance().addEvent(event);
            return true;
        }
        return false;
    }

    Event* getEventIdFromUser(){
        bool isValidId = true;
        string errorMsg = "";
        int eventId = 0;

        vector<Field> EventIdField = {
            {
                "Event ID:", 10, "0-9",
                [](void* obj, const char* v) {
                    *static_cast<int*>(obj) = atoi(v);
                }
            }
        };

        do {
            if (!showForm(&eventId, EventIdField, errorMsg)) return nullptr;
            errorMsg = "";
            Event* e = EventManager::getInstance().getEvent(eventId);
            if (e != nullptr) return e;
            else {
                isValidId = false;
                errorMsg = "Event ID is not found";
            }
        } while(!isValidId);
        return nullptr;
    }

    bool viewEditEventForm(Event* event) {
        vector<EventField>& eventFields = createEventFormFields();
        fillEditEventData(eventFields, *event);
        vector<Field> fields(
            make_move_iterator(eventFields.begin()),
            make_move_iterator(eventFields.end())
        );
        if (showForm(event, fields, "", 0, 30)) return true;
        return false;
    }

    int viewEventsPage() {
        EventManager& eventManager = EventManager::getInstance();
        vector<string> eventsMenu;

        // Loop through events
        for(int i = 0; i < eventManager.getEvents().size(); i++)
        {
            Event tempEvent = eventManager.getEvents()[i];
            eventsMenu.push_back(tempEvent.viewDetailsBreifly());
        }

        int selectedEvent = displayMenu(eventsMenu, "Current Events");

        // if user clicks ESC
        if(selectedEvent == -1)
        {
            return -1;
        }

        // when user chooses event, then make him choose ticket type of event and also show event details
        int selectedTicketType = displayMenu(
            vector<string>{"1-VIP\n", "2-Economic\n", "3-Regular\n"},
            "Choose your ticket type",
            "Event details",
            eventManager.getEvent(selectedEvent)->viewDetails(),
            15
        );

        // if user clicks ESC
        if(selectedTicketType == -1)
        {
            return -1;
        }

        // navigate to purchase page
        TicketTypePrice selectedTicketTypePrice;
        switch (selectedTicketType) {
            case 1:
                selectedTicketTypePrice.type = TicketType::VIP;
                selectedTicketTypePrice.price = eventManager.getEvents()[selectedEvent-1].getVipTickets().price;
                break;
            case 2:
                selectedTicketTypePrice.type = TicketType::Economic;
                selectedTicketTypePrice.price = eventManager.getEvents()[selectedEvent-1].getEconomicTickets().price;
                break;
            case 3:
                selectedTicketTypePrice.type = TicketType::Regular;
                selectedTicketTypePrice.price = eventManager.getEvents()[selectedEvent-1].getRegularTickets().price;
                break;
        }

        purchasePage(selectedEvent, selectedTicketTypePrice);

        return 0;
    }

    int viewAdminMenu(){
        if (!isAdmin()) return -1;

        vector<string> adminOptions = {
            "1- Create Event\n",
            "2- Edit Event\n",
            "3- Delete Event\n",
            "4- Log out\n"
        };

        while (true) {
            int choice = displayMenu(
                adminOptions,
                "======= Admin Menu =======",
                "Welcome, " + currentAdmin->getName(),
                "",  
                6
            );

            switch (choice) {
            case 1:
                viewCreateEventForm();
                break;
            case 2:{
                Event* e = getEventIdFromUser();
                if (e == nullptr) break;
                viewEditEventForm(e);
                break;
            }
            case 3:
                break;    
            case 4:
                logout();
            case -1:
                return -1;
            }
        }
    }

    int viewMyTicketsPage() {
        if (!currentFan) return -1;

        vector<string> ticketOptions = currentFan->getTicketsMenuItems();

        if (ticketOptions.size() == 1 && currentFan->viewMyTickets().empty()) {
            displayMenu(ticketOptions, "====== My Tickets ======");
            return 0;
        }

        while (true) {
            int choice = displayMenu(
                ticketOptions,
                "====== My Tickets ======",
                "Select a ticket to view details",
                "",
                5
            );

            if (choice == -1) {
                return 0;
            }

            string ticketDetails = currentFan->getTicketDetails(choice - 1);

            vector<string> backOption = {"Press ESC to go back\n"};
            displayMenu(backOption, "====== Ticket Details ======", "", ticketDetails, 15);
        }

        return 0;
    }

    int viewFanMenu()
    {
        if (!isFan()) return -1;

        vector<string> fanOptions = {
            "1- View Events\n",
            "2- My Tickets\n",
            "3- Logout\n"
        };

        while (true) {
            int choice = displayMenu(
                fanOptions,
                "======= Fan Menu =======",
                "Welcome, " + currentFan->getName(),
                "", 
                6
            );

            switch (choice) {
                case 1:
                    viewEventsPage();
                    break;
                case 2:
                    viewMyTicketsPage();
                    break;
                case 3:
                    logout();
                case -1:
                    return -1;
            }
        }
    }

    bool viewRegisterForm() {
        vector<Field> registerFormFields = {
                {
                        "Name:",        20, "A-Za-z ",
                        [](void *user, const char *nameInput) {
                            ((Fan *) user)->setName(nameInput);
                        }
                },
                {
                        "Email:",       50, EMAIL_ALLOWED_CHARS,
                        [](void *user, const char *emailInput) {
                            ((Fan *) user)->setEmail(emailInput);
                        }
                },
                {
                        "Password:",    50, " -~",
                        [](void *user, const char *pass) {
                            ((Fan *) user)->setPassword(pass);
                        }
                },
                {
                        "Gender(M|F):", 1,  "MFmf",
                        [](void *user, const char *gender) {
                            ((Fan *) user)->setGender(toupper(gender[0]));
                        }
                },
                {
                        "Phone:",       11, "0-9",
                        [](void *user, const char *phone) {
                            ((Fan *) user)->setPhoneNumber(phone);
                        }
                }
        };

        Fan fan;
        string errorMsg = "";
        bool cancelForm = false;
        int errorCount = 0;
        do {
            if (!showForm(&fan, registerFormFields, errorMsg, errorCount))
                cancelForm = true;

            // reset the errors
            errorCount = 0;
            errorMsg = "";

            if (!ValidationService::isValidEmail(fan.getEmail())) {
                errorMsg += "\nEmail is not valid.";
                ++errorCount;
            }

            if (AuthenticationService::isExistingEmail(fan.getEmail())) {
                errorMsg += "\nEmail is already in use.";
                ++errorCount;
            }

            if (!ValidationService::isValidUserName(fan.getName())) {
                errorMsg += "\nThe Name Should be from 4 to 20 characters.";
                ++errorCount;
            }

            if (!ValidationService::isValidPassword(fan.getPassword())) {
                errorMsg += "\nThe Password Should be at least 8 characters.";
                ++errorCount;
            }

            if (!errorCount) {
                if (AuthenticationService::_register(fan)) return true;
            }
        } while (!cancelForm);
        return false;
    }

    vector<Event> searchEventsByCategory(Category category) {
        EventManager &eventManager = EventManager::getInstance();
        vector<Event> allEvents = eventManager.getEvents();
        vector<Event> matchedEvents;
        for (const Event &event: allEvents) {
            if (event.getCategory() == category) {
                matchedEvents.push_back(event);
            }
        }
        return allEvents;
    }

    bool purchasePage(int selectedEventId , TicketTypePrice selectedTicketTypePrice)
    {
        int selectedPaymentMethod = displayMenu(
            vector<string>{"1-Fawry Pay\n", "2-Credit Card\n"},
            "Choose your payment method",
            "Ticket price ", "  " + to_string(selectedTicketTypePrice.price), 7
        );
        PaymentService paymentService;
        system("cls");
        // handle ESC case
        if(selectedPaymentMethod == -1)
        {
            return false;
        }
        // User select to pay with Fawry
        else if(selectedPaymentMethod == 1)
        {
            PaymentMethod* fawry = new FawryPay();
            paymentService.setPaymentMethod(fawry);
        }
        // User select to pay with Credit Card
        else if(selectedPaymentMethod == 2)
        {
            vector<Field> creditCardFields = {
                {
                    "Cardholder Name:", 30, "A-Za-z ",
                    [](void* cc, const char* input) {
                        ((CreditCard*)cc)->setName(input);
                    }
                },
                {
                    "Card Number:", 16, "0-9",
                    [](void* cc, const char* input) {
                        ((CreditCard*)cc)->setCardNumber(input);
                    }
                },
                {
                    "CVV:", 4, "0-9",
                    [](void* cc, const char* input) {
                        ((CreditCard*)cc)->setCvv(input);
                    }
                },
                {
                    "Exp Date(MM-YYYY):", 7, "0-9-",
                    [](void* cc, const char* input) {
                        ((CreditCard*)cc)->setExpiryDate(input);
                    }
                }
            };
            PaymentMethod* creditCard = new CreditCard("","","","");
            if (!showForm(creditCard, creditCardFields)) {
                cout << "Credit card entry canceled!\n";
                return false;
            }
            system("cls");
            paymentService.setPaymentMethod(creditCard);
        }

        // pay for ticket
        paymentService.processPayment(selectedTicketTypePrice.price);

        // after pay for ticket we will book ticket for that event
        EventManager& eventManager = EventManager::getInstance();
        Event* event = eventManager.getEvent(selectedEventId);
        Ticket createdTicket = event->bookEvent(currentFan->getId() , selectedTicketTypePrice);
        // case booking is failed
        if(createdTicket.getId() == "0")
        {
            display(43,"Unavailable tickets please try again later.",43,0,2,50);
            return false;
        }
        // Add created ticket to current fan tickets
        currentFan->buyTicket(createdTicket);
        return true;
    }

    int viewLoginForm() {
        vector<Field> loginFormFields = {
                {
                        "Email:",    50, EMAIL_ALLOWED_CHARS,
                        [](void *user, const char *emailInput) {
                            ((LoginDTO *) user)->email = emailInput;
                        }
                },
                {
                        "Password:", 50, " -~",
                        [](void *user, const char *pass) {
                            ((LoginDTO *) user)->password = pass;
                        }
                }
        };

        bool abortLoginFormFill = false;
        do {
            // Choosing User Type Menu
            int user_type = displayMenu(vector<string>{"1-Fan\n", "2-Admin\n"}, "You want sign in as");
            // if user Press on ESC to back to main menu
            if (user_type == -1) { return -1; }

            User *currentUser = nullptr;

            string errorMsg = "";
            do {
                LoginDTO user;
                if (!showForm(&user, loginFormFields, errorMsg)) {
                    // if user press on ESC to return to Choosing User Type Menu
                    abortLoginFormFill = true;
                    break;
                }

                UserType userType = static_cast<UserType>(user_type);

                currentUser = AuthenticationService::login(user, userType);
                if (currentUser != nullptr) {
                    if (userType == UserType::Fan) {
                        Fan *fan = static_cast<Fan *>(currentUser);
                        setCurrentFan(*fan);
                        return 1;
                    } else if (userType == UserType::Admin) {
                        Admin *admin = static_cast<Admin *>(currentUser);
                        setCurrentAdmin(*admin);
                        return 2;
                    }
                } else {
                    errorMsg = "Your credentials are wrong, please try again.";
                }
            } while (currentUser == nullptr);

        } while (abortLoginFormFill);

        return -1;
    }
};


void SystemManager::run() {
    vector<string> menu = {"1- Login\n", "2- Register\n",};

    while (true) {
        int choice = displayMenu(menu, "====================Welcome to Ticketak======================");

        switch (choice) {
        // Login
        case 1: {

            int result = viewLoginForm();

            switch (result) {
                // Returning to main menu
            case -1:
                break;
            // Fan Logs in
            case 1:{
                viewFanMenu();
                break;
            }
            // Admin Logs in
            case 2: {
                viewAdminMenu();
                break;
            }
            }
            break;
        }
        case 2: { // Register
            viewRegisterForm();
            break;
        }
        case -1: {
            system("cls");
            cout << "Thanks for using Ticketak :)\n";
            return;
        }
        }
    }
}


// ================= MAIN (ENTRY POINT) =================

int main() {
    Admin admin("Karim", "admin@ticketak.com", "password", 'M', "01065243880");
    //Admin admin("Karim", "a", "p", 'M', "01065243880");
    AdminManager::getInstance().addAdmin(admin);

    EventManager& eventManager = EventManager::getInstance();

    if (eventManager.getEvents().empty()) {
        TicketTypePriceQuantity vip1{TicketType::VIP, 500.0, 50};
        TicketTypePriceQuantity eco1{TicketType::Economic, 200.0, 150};
        TicketTypePriceQuantity reg1{TicketType::Regular, 100.0, 300};
        Date date1{10, 1, 2026};

        TicketTypePriceQuantity vip2{TicketType::VIP, 1000.0, 30};
        TicketTypePriceQuantity eco2{TicketType::Economic, 400.0, 70};
        TicketTypePriceQuantity reg2{TicketType::Regular, 150.0, 200};
        Date date2{15, 6, 2026};

        TicketTypePriceQuantity vip3{TicketType::VIP, 800.0, 40};
        TicketTypePriceQuantity eco3{TicketType::Economic, 300.0, 120};
        TicketTypePriceQuantity reg3{TicketType::Regular, 120.0, 250};
        Date date3{20, 12, 2026};

        Event event1(1, "Rock Concert", Category::Parties, date1, vip1, eco1, reg1);
        Event event2(2, "Football Match", Category::Sports, date2, vip2, eco2, reg2);
        Event event3(3, "City Carnival", Category::Carnivals, date3, vip3, eco3, reg3);

        eventManager.addEvent(event1);
        eventManager.addEvent(event2);
        eventManager.addEvent(event3);
    }

    SystemManager app;
    app.run();
    return 0;
}
