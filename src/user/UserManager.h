#pragma once

#include<string>
#include <unordered_map>
#include "Role.h"
#include "User.h"


class UserStore;

class UserManager {
private:

    std::unordered_map<int, User> users;
    std::unordered_map<std::string, int> usernameToId;
    UserStore* store;
    User* currentUser = nullptr;
    int nextUserId=1;

public:
    UserManager(UserStore* store);

    bool createUser(int userId, const std::string& username,const std::string& password,Role role);

    User* getUser(int userId);

    bool userExists(int userId) const;

    bool login(const std::string& username,const std::string& password);

    void logout();

    User* getCurrentUser() const;
    int generateUserId();
    bool deleteUser(int userId);
};
