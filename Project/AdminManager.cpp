#include <vector>
#include <string>
#include <algorithm>
#include "Admin.cpp"

// Singleton Class for Admins
class AdminManager {
private:
    vector<Admin> admins;

    // Private constructor
    AdminManager() = default;

    // Disable copy & assignment
    AdminManager(const AdminManager&) = delete;
    AdminManager& operator=(const AdminManager&) = delete;

public:
    static AdminManager& getInstance() {
        static AdminManager instance; // Magic Static
        return instance;
    }

    void addAdmin(const Admin& admin) {
        admins.push_back(admin);
    }

    const vector<Admin>& getAdmins() const {
        return admins;
    }

    Admin* getAdminByEmailPass(string email, string password) {
        auto it = find_if(admins.begin(), admins.end(),
            [email, password](Admin& admin) {
                return (admin.getEmail() == email && admin.getPassword() == password);
            }
        );

        if (it != admins.end())
            return &(*it);

        return nullptr;
    }
};
