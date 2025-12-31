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
    }

    static bool isValidPassword(const string &password) {
        return (password.size() >= 8);
    }

    static int isValidEvent(Event& e,string &error) {
        int errC = 0;
        if (e.getName().empty()) {
            error = "Event name cannot be empty";
            errC++;
        }
        
        if (e.getDay() < 1 || e.getDay() > 31) {
            if (!error.empty()) error += '\n';
            error += "Invalid day";
            errC++;
        }
        
        if (e.getMonth() < 1 || e.getMonth() > 12) {
            if (!error.empty()) error += '\n';
            error += "Invalid month";
            errC++;
        }
        
        if (e.getYear() < 2025) {
            if (!error.empty()) error += '\n';
            error += "Invalid year";
            errC++;
        }

        if (e.getEventStatus() == EventStatus::Finished) {
            if (!error.empty()) error += '\n';
            error += "Date Shouldn't be in the past";
            errC++;
        }

        const auto& vipTickets = e.getVipTickets();
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

        const auto& regularTickets = e.getRegularTickets();
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

        const auto& economicTickets = e.getEconomicTickets();
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

    static bool isExistingEmail(const string &email) {
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

    void setName(const string &n) { name = n; }

    void setCardNumber(const string &num) { cardNumber = num; }

    void setCvv(const string &c) { cvv = c; }

    void setExpiryDate(const string &exp) { expiryDate = exp; }

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
public:
    void run(); // Main loop

private:
    Admin *currentAdmin = nullptr;
    Fan *currentFan = nullptr;
    UserType userType = UserType::NotAuth;
    const string EMAIL_ALLOWED_CHARS = "A-Za-z0-9_@.%+-";

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

    vector<EventField> &createEventFormFields() {
        static vector<EventField> fields = {
                {
                        {
                                "Event Name:",
                                30,
                                "A-Za-z ",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setName(v);
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
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setDay(atoi(v));
                                }
                        },
                        EventFieldId::DateDay
                },
                {
                        {
                                "Month:",
                                2,
                                "0-9",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setMonth(atoi(v));
                                }
                        },
                        EventFieldId::DateMonth
                },
                {
                        {
                                "Year:",
                                4,
                                "0-9",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setYear(atoi(v));
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
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketPrice(TicketType::VIP, atof(v));
                                }
                        },
                        EventFieldId::VipPrice
                },
                {
                        {
                                "VIP Ticket Quantity:",
                                6,
                                "0-9",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketQuantity(TicketType::VIP, atoi(v));
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
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketPrice(TicketType::Regular, atof(v));
                                }
                        },
                        EventFieldId::RegularPrice
                },
                {
                        {
                                "Regular Ticket Quantity:",
                                6,
                                "0-9",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketQuantity(TicketType::Regular, atoi(v));
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
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketPrice(TicketType::Economic, atof(v));
                                }
                        },
                        EventFieldId::EconomicPrice
                },
                {
                        {
                                "Economic Ticket Quantity:",
                                6,
                                "0-9",
                                [](void *obj, const char *v) {
                                    static_cast<Event *>(obj)->setTicketQuantity(TicketType::Economic, atoi(v));
                                }
                        },
                        EventFieldId::EconomicQuantity
                }
        };

        return fields;
    }

    //function to display category menu and get user selection
    Category getCategoryFromUser(const string& prompt = "Choose Event Category") {
        vector<string> categories = {
            "1- Sports\n",
            "2- Parties\n",
            "3- Carnivals\n",
            "4- Other\n"
        };

        int choice = displayMenu(
            categories,
            "======= Event Categories =======",
            prompt,
            "",
            6
        );

        // If user presses ESC, return -1 cast to Category to signal ESC press
        return static_cast<Category>(choice);
    }

    void fillEditEventData(vector<EventField> &fields, const Event &event) {
        for (auto &f: fields) {
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
        vector<EventField> &eventFields = createEventFormFields();
        vector<Field> fields(eventFields.begin(), eventFields.end());
        string error;
        int errC = 0;
        Event event;

        while (true) {
            Category selectedCategory = getCategoryFromUser("Select Category for New Event");

            if (selectedCategory == static_cast<Category>(-1)) return false;


            while (true) {
                if (!showForm(&event, fields, error, errC, 30))
                    break;

                error = "";
                errC = ValidationService::isValidEvent(event, error);
                if (!errC) {
                    event.setCategory(selectedCategory);
                    EventManager::getInstance().addEvent(event);
                    system("cls");
                    cout << "Event #" << EventManager::getInstance().getNEvents() << " is created successfully";
                    Sleep(1500);
                    return true;
                }
            }
        }
    }

    Event *getEventIdFromUser() {
        bool isValidId = true;
        string errorMsg = "";
        int eventId = 0;

        vector<Field> EventIdField = {
                {
                        "Event ID:", 10, "0-9",
                        [](void *obj, const char *v) {
                            *static_cast<int *>(obj) = atoi(v);
                        }
                }
        };

        do {
            if (!showForm(&eventId, EventIdField, errorMsg, 0 , 15)) return nullptr;
            errorMsg = "";
            Event *e = EventManager::getInstance().getEvent(eventId);
            if (e != nullptr) return e;
            else {
                isValidId = false;
                errorMsg = "Event ID is not found";
            }
        } while (!isValidId);
        return nullptr;
    }

    bool viewEditEventForm(Event *event) {
        vector<EventField> &eventFields = createEventFormFields();
        string error;
        int errC = 0;

        while (true) {
            Category currentCategory = event->getCategory();

            Category newCategory = getCategoryFromUser(
                "Current: " + event->categoryToString(currentCategory) + "\nSelect new category or ESC to keep"
            );

            if (newCategory == static_cast<Category>(-1)) return false;

            event->setCategory(newCategory);

            fillEditEventData(eventFields, *event);
            vector<Field> fields(eventFields.begin(), eventFields.end());

            while(true){
                if (!showForm(event, fields, error, errC, 30))
                    break;

                errC = ValidationService::isValidEvent(*event, error);
                if (!errC) {
                    // set
                    system("cls");
                    cout << "Event #" << event->getId() << " is edited successfully";
                    Sleep(1500);
                    return true;
                }
            }
        }
    }

    void getEventsMenu(vector<string>& eventsMenu, const vector<Event>& events){
        for (const Event& e : events){
            eventsMenu.push_back(e.viewDetailsBreifly());
        }
    }

    // View Events Page to Fan to purchase
    int viewEventsForPurchase() {
        EventManager &eventManager = EventManager::getInstance();
        vector<string> eventsMenu;
        const vector<Event>& events = eventManager.getEvents();
        int selectedTicketType = 0;
        int selectedEvent = 0;

        // getEventsMenu
        getEventsMenu(eventsMenu, events);

        while (true) {
            selectedEvent = displayMenu(eventsMenu, "Current Events");

            // if user clicks ESC
            if (selectedEvent == -1){
                return -1;
            }

            while (true) {
                // when user chooses event, then make him choose ticket type of event and also show event details
                selectedTicketType = displayMenu(
                    vector<string>{"1-VIP\n", "2-Economic\n", "3-Regular\n"},
                    "Choose your ticket type",
                    "Event details",
                    events[selectedEvent - 1].viewDetails(),
                    17
                );

                // if user clicks ESC
                if (selectedTicketType == -1){
                    break;
                }

                if (events[selectedEvent-1].getEventStatus() == EventStatus::Finished) {
                    displayMenu(vector<string>(), "Sorry, This Event is already Finished\n");
                    continue;
                }

                // navigate to purchase page
                TicketTypePrice selectedTicketTypePrice;
                switch (selectedTicketType) {
                    case 1:
                        selectedTicketTypePrice.type = TicketType::VIP;
                        selectedTicketTypePrice.price = events[selectedEvent-1].getVipTickets().price;
                        break;
                    case 2:
                        selectedTicketTypePrice.type = TicketType::Economic;
                        selectedTicketTypePrice.price = events[selectedEvent-1].getEconomicTickets().price;
                        break;
                    case 3:
                        selectedTicketTypePrice.type = TicketType::Regular;
                        selectedTicketTypePrice.price = events[selectedEvent-1].getRegularTickets().price;
                        break;
                }
                
                if (!purchasePage(events[selectedEvent-1].getId(), selectedTicketTypePrice)) {
                    continue;
                }
                return 0;
            }
        }
        return 0;
    }

    bool purchasePage(int selectedEventId, TicketTypePrice selectedTicketTypePrice) {
        PaymentService paymentService;

        while (true) {
            int selectedPaymentMethod = displayMenu(
                    vector<string>{"1-Fawry Pay\n", "2-Credit Card\n"},
                    "Choose your payment method",
                    "Ticket price ", "  " + to_string(selectedTicketTypePrice.price), 7
            );

            system("cls");
            // handle ESC case
            if (selectedPaymentMethod == -1){
                return false;
            }
            // User select to pay with Fawry
            else if (selectedPaymentMethod == 1) {
                PaymentMethod *fawry = new FawryPay();
                paymentService.setPaymentMethod(fawry);
                break;
            }
            // User select to pay with Credit Card
            else if (selectedPaymentMethod == 2) {
                vector<Field> creditCardFields = {
                        {
                                "Cardholder Name:",   30, "A-Za-z ",
                                [](void *cc, const char *input) {
                                    ((CreditCard *) cc)->setName(input);
                                }
                        },
                        {
                                "Card Number:",       16, "0-9",
                                [](void *cc, const char *input) {
                                    ((CreditCard *) cc)->setCardNumber(input);
                                }
                        },
                        {
                                "CVV:",               4,  "0-9",
                                [](void *cc, const char *input) {
                                    ((CreditCard *) cc)->setCvv(input);
                                }
                        },
                        {
                                "Exp Date(MM-YYYY):", 7,  "0-9-",
                                [](void *cc, const char *input) {
                                    ((CreditCard *) cc)->setExpiryDate(input);
                                }
                        }
                };
                PaymentMethod *creditCard = new CreditCard("", "", "", "");
                if (!showForm(creditCard, creditCardFields , "" , 0, 21)) {
                    cout << "Credit card entry canceled!\n";
                    continue;
                }
                system("cls");
                paymentService.setPaymentMethod(creditCard);
                break;
            }
        }

        // pay for ticket
        paymentService.processPayment(selectedTicketTypePrice.price);

        // after pay for ticket we will book ticket for that event
        EventManager &eventManager = EventManager::getInstance();
        Event *event = eventManager.getEvent(selectedEventId);
        Ticket createdTicket = event->bookEvent(currentFan->getId(), selectedTicketTypePrice);
        // case booking is failed
        if (createdTicket.getId() == "0") {
            displayMenu(vector<string>(), "Unavailable tickets please try again later.");
            return false;
        }
        // Add created ticket to current fan tickets
        currentFan->buyTicket(createdTicket);
        system("cls");
        cout << "Payment is Completed Successfully, Backing to Main Menu in 1 Sec.";
        Sleep(1200);
        return true;
    }

    int viewAdminMenu() {
        if (!isAdmin()) return -1;

        vector<string> adminOptions = {
                "1- Create Event\n",
                "2- Edit Event\n",
                "3- Delete Event\n",
                "4- View Events\n",
                "5- Search For Event\n",
                "6- Log out\n"
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
                case 2: {
                    Event *e = getEventIdFromUser();
                    if (e == nullptr) break;
                    viewEditEventForm(e);
                    break;
                }
                case 3: {
                    Event *e = getEventIdFromUser();
                    if (e == nullptr) break;
                    int eventID = e->getId();
                    if (EventManager::getInstance().deleteEvent(eventID)){
                        system("cls");
                        cout << "Event #"<< eventID << " is deleted successfully";
                        Sleep(1500);
                    }
                }
                case 4:{
                    viewEvents(EventManager::getInstance().getEvents(),"====== All Events ======");
                    break;
                }
                case 5:
                    searchMenu();
                    break;
                case 6: {
                    logout();
                    return -1;
                }
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

            if(displayMenu(vector<string>(), ticketDetails, "", "", 8) == -1)
            {
             continue;
            }
            return 0;
        }
        return 0;
    }

    int searchMenu() {
        vector<string> searchOptions = {
            "1- Search By Name\n",
            "2- Search By Category\n",
            "3- Search By ID\n"
        };

        while (true) {
            int choice = displayMenu(searchOptions, "======= Search For Event =======");
            vector<Event> matchedEvents;
            bool exit = false;
            switch(choice){
                case 1: {
                    string name;
                    vector<Field> nameField {{
                        "Event Name:",
                        30,
                        "A-Za-z ",
                        [](void* obj, const char* v) {
                            *static_cast<string*>(obj) = v;
                        }
                    }};
                    if (!showForm(&name, nameField)) {exit = true; break;}
                    matchedEvents = searchEventsByName(name);
                    break;
                }
                case 2: {
                    // Make Function to return Category => categoryMenu()
                    Category selectedCategory = getCategoryFromUser("Select Category to Search");

                    //Check if user pressed ESC
                    if (selectedCategory == static_cast<Category>(-1))
                    {
                        exit = true;
                        break;
                    }

                    matchedEvents = searchEventsByCategory(selectedCategory);
                    break;
                }
                case 3: {
                    Event* event = getEventIdFromUser();
                    if (event == nullptr) {exit = true; break;}
                    matchedEvents.push_back(*event);
                    break;
                }
                case -1:
                    return -1;
            }

            if (exit) continue;

            //Check if no events found
            if (matchedEvents.empty()) {
            system("cls");
            cout << "No events found matching your search criteria.\n";
            Sleep(2000);
            continue;
        }

            viewEvents(matchedEvents, "Search Results");
        }
    }

    // View Passed Events
    void viewEvents(const vector<Event>& events,const string& menuMsg){
        if (!events.empty()) {
            vector<string> eventsMenu;
            getEventsMenu(eventsMenu, events);

            while (true) {
                int selectedEventIndex = displayMenu(eventsMenu, menuMsg);

                // Back to Search Menu
                if (selectedEventIndex == -1){
                    break;
                }

                displayMenu(vector<string>(), "====== Event Details ======",
                    events[selectedEventIndex - 1].viewDetails(),
                    "", 16
                );
            }
        }
    }

    int viewFanMenu(){
        if (!isFan()) return -1;

        vector<string> fanOptions = {
            "1- Explore Events\n",
            "2- My Tickets\n",
            "3- Search for Event\n",
            "4- Logout\n"
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
                viewEventsForPurchase();
                break;
            case 2:
                viewMyTicketsPage();
                break;
            case 3:
                searchMenu();
                break;
            case 4:
                logout();
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
            if (!showForm(&fan, registerFormFields, errorMsg, errorCount, 21))
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
                if (AuthenticationService::_register(fan)) {
                    system("cls");
                    cout << "Registration is done successfully, Forwarding to Login Page in 2 sec\n";
                    Sleep(2000);
                    return true;
                }
            }
        } while (!cancelForm);
        return false;
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
            int user_type = displayMenu(vector<string>{"1- Fan\n", "2- Admin\n"}, "You want sign in as");
            // if user Press on ESC to back to main menu
            if (user_type == -1) { return -1; }

            User *currentUser = nullptr;

            string errorMsg = "";
            do {
                LoginDTO user;
                if (!showForm(&user, loginFormFields, errorMsg, 0, 15)) {
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

    vector<Event> searchEventsByCategory(Category category) {
        EventManager &eventManager = EventManager::getInstance();
        vector<Event> allEvents = eventManager.getEvents();
        vector<Event> matchedEvents;
        for (const Event &event: allEvents) {
            if (event.getCategory() == category) {
                matchedEvents.push_back(event);
            }
        }
        return matchedEvents;
    }

    vector<Event> searchEventsByName(const string& name) {
        const vector<Event>& allEvents = EventManager::getInstance().getEvents();
        vector<Event> matchedEvents;

        // helper function to lowercase a string
        auto toLower = [](string s) {
            transform(s.begin(), s.end(), s.begin(), ::tolower);
            return s;
        };

        for (const Event &event: allEvents) {
            // if Event Name Contains the input name (case insensitive) then push it into matchedEvents
            if (toLower(event.getName()).find(toLower(name)) != string::npos) {
                matchedEvents.push_back(event);
            }
        }
        return matchedEvents;
    }
};


void SystemManager::run() {
    vector<string> menu = {"1- Login\n", "2- Register\n",};

    while (true) {
        int choice = displayMenu(menu, "====================Welcome to Ticketak======================");

        switch (choice) {
            case 2: { // Register
                if (!viewRegisterForm()) break;
                // else forwarding to login page
            }
            // Login
            case 1: {

                int result = viewLoginForm();

                switch (result) {
                    // Returning to main menu
                    case -1:
                        break;
                    // Fan Logs in
                    case 1: {
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
    //Admin admin("Karim", "admin@ticketak.com", "password", 'M', "01065243880");
    Admin admin("Karim", "a", "p", 'M', "01065243880");
    AdminManager::getInstance().addAdmin(admin);

    Fan fan("Ahmad", "f", "p", 'M', "01065246880");
    FanManager::getInstance().addFan(fan);

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
        Date date3{20, 12, 2025};

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
