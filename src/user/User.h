#pragma once

#include "Role.h"
#include "SearchHistory.h"   
#include<string>

class Query;

class User{
private:
    int userId;
    std::string userName;
    std::string password;
    Role role;


    SearchHistory searchHistory;

public:
    User(int userId,const std::string& userName,const std::string& password,Role role);

    int getUserId() const;

    const std::string getUserName()const;
    const std::string& getPassword() const;
    Role getRole()const;
    bool isAdmin() const;

    SearchHistory& getSearchHistory() ;

};

