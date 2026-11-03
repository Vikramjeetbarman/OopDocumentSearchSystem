#include "FileUserStore.h"
#include "User.h"
#include <fstream>
#include <stdexcept>
#include <sstream>



FileUserStore::FileUserStore(const std::string& filePath)
    : filePath(filePath) {
        
    }

void FileUserStore::saveUser(const User& user) {
    std::ofstream fout(filePath, std::ios::app);

    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open users file for writing");
    }


    std::string roleStr =(user.getRole() == Role::USER) ? "USER": "ADMIN";

    fout << user.getUserId() << ","
        << user.getUserName() << ","
        << user.getPassword() << ","
        << roleStr
        << "\n";

    fout.close();
}

std::vector<User> FileUserStore::loadAllUsers() {
    std::vector<User> users;

    std::ifstream fin(filePath);
    
    if (!fin.is_open()) {
        throw std::runtime_error("Failed to open users file for reading");
    }

    std::string line;

    while (std::getline(fin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, name,password, roleStr;

        std::getline(ss, idStr, ',');
        std::getline(ss, name, ',');
        std::getline(ss, password, ',');
        std::getline(ss, roleStr);

        int id = std::stoi(idStr);
        Role role;

        if (roleStr == "USER")
            role = Role::USER;
        else
            role = Role::ADMIN;

        users.emplace_back(id, name,password, role);
    }

    fin.close();
    

    return users;
}

void FileUserStore::deleteUser(int userId) {

    std::vector<User> users = loadAllUsers();

    std::ofstream fout(filePath, std::ios::trunc);

    if (!fout.is_open())
        throw std::runtime_error("Failed to open users file");

    for (const User& user : users) {

        if (user.getUserId() == userId)
            continue;

        std::string roleStr =
            (user.getRole() == Role::USER) ? "USER" : "ADMIN";

        fout << user.getUserId() << ","
             << user.getUserName() << ","
             << user.getPassword() << ","
             << roleStr
             << "\n";
    }
}

