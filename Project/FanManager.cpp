#include <vector>
#include <string>
#include <algorithm>
#include "Fan.cpp"

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
    static FanManager& getInstance() {
        static FanManager instance; // Magic Static
        return instance;
    }

    void addFan(const Fan& e) {
        fans.push_back(e);
    }

    const vector<Fan>& getFans() const {
        return fans;
    }

    Fan* getFan(int ID) {
        auto it = find_if(fans.begin(), fans.end(),
            [ID](Fan& e) {
                return e.getId() == ID;
            }
        );

        if (it != fans.end())
            return &(*it);

        return nullptr;
    }
};