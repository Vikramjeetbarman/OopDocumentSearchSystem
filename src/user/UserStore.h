#pragma once

#include <vector>

class User;

class UserStore {
public:
    virtual ~UserStore() = default;

    virtual void saveUser(const User& user) = 0;

    virtual std::vector<User> loadAllUsers() = 0;

    virtual void deleteUser(int userId) = 0;
};
