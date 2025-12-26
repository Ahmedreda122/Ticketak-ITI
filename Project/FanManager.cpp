#include "Fan.cpp"
#include <string>
#include <algorithm>

// Singleton Class for Fans
class FanManager {
private:
    vector<Fan> fans;

    // Private constructor
    FanManager() = default;

    // Disable copy & assignment
    FanManager(const FanManager&) = delete;
    FanManager& operator=(const FanManager&) = delete;

public:
    int getSize(){
        return fans.size();
    }

    static FanManager& getInstance() {
        static FanManager instance; // Magic Static
        return instance;
    }

    void addFan(const Fan& fan) {
        fans.push_back(fan);
    }

    const vector<Fan>& getFans() const {
        return fans;
    }

    Fan* getFanByEmail(string email) {
        auto it = find_if(fans.begin(), fans.end(),
            [email](Fan& f) {
                return (f.getEmail() == email);
            }
        );

        if (it != fans.end())
            return &(*it);

        return nullptr;
    }

    Fan* getFanByEmailPass(string email, string password) {
        auto it = find_if(fans.begin(), fans.end(),
            [email, password](Fan& f) {
                return (f.getEmail() == email && f.getPassword() == password);
            }
        );

        if (it != fans.end())
            return &(*it);

        return nullptr;
    }

    Fan* getFan(int ID) {
        auto it = find_if(fans.begin(), fans.end(),
            [ID](Fan& f) {
                return f.getId() == ID;
            }
        );

        if (it != fans.end())
            return &(*it);

        return nullptr;
    }
};