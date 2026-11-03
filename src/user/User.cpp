#include "User.h"
#include "SearchHistory.h"
#include "../core/Query.h"


User::User(int userId,const std::string& userName,const std::string& password,Role role)
:userId(userId),userName(userName),password(password),role(role){}

int User::getUserId() const{
    return userId;
}

const std::string User::getUserName()const{
    return userName;
}

const std::string& User::getPassword() const {
    return password;
}

Role User::getRole()const{
    return role;
}

bool User::isAdmin() const {
    return role == Role::ADMIN;
}

SearchHistory& User::getSearchHistory() {
    return searchHistory;
}



