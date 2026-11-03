#pragma once

#include "UserStore.h"
#include <string>

// File-based implementation of UserStore
class FileUserStore : public UserStore {
private:
    std::string filePath;
    

public:
    explicit FileUserStore(const std::string& filePath);

    void saveUser(const User& user) override;

    std::vector<User> loadAllUsers() override;
    
    void deleteUser(int userId) override;
};
