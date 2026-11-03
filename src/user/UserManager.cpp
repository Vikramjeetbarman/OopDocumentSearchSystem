
#include "UserManager.h"
#include "User.h"
#include "UserStore.h"
#include "Role.h"

UserManager::UserManager(UserStore* store)
    : store(store) {

    auto loadedUsers = store->loadAllUsers();

    for (const auto& user : loadedUsers) {
        users.emplace(user.getUserId(), user);
        usernameToId.emplace(user.getUserName(),user.getUserId());
    }



    nextUserId = 1;

    for (const auto& pair : users) {
        nextUserId = std::max(nextUserId, pair.first + 1);
    }
}

bool UserManager::createUser(int userId, const std::string& username,const std::string& password,Role role){
    User user(userId,username,password,role);

    if (getUser(userId)!=nullptr)
        return false;

    if(usernameToId.find(username) != usernameToId.end())
    {
        return false;
    }

    users.emplace(userId, user);
    usernameToId.emplace(user.getUserName(), user.getUserId());
    store->saveUser(user);
    return true;

}
    
User* UserManager::getUser(int userId){
    auto it =users.find(userId);
    if(it==users.end())
        return nullptr; 
    return &(it->second);
}

bool UserManager::userExists(int userId) const{
    return users.find(userId)!=users.end();
}

bool UserManager::login(const std::string& username,
                        const std::string& password)
{
     auto it = usernameToId.find(username);
    if (it == usernameToId.end())
        return false;
    User* user = getUser(it->second);
    if (user==nullptr)
        return false;

    if (user->getPassword() != password)
        return false;

    currentUser = user;
    return true;
}


void UserManager::logout() {
    currentUser = nullptr;
}

User* UserManager::getCurrentUser() const {
    return currentUser;
}

bool UserManager::deleteUser(int userId) {

    User* current = getCurrentUser();

    if (!current)
    return false;

if (!current->isAdmin() &&
    current->getUserId() != userId)
{
    return false;
}

    auto it = users.find(userId);
    if (it == users.end())
        return false;
    
    if (it->second.getRole() == Role::ADMIN)
        return false;
    usernameToId.erase(it->second.getUserName());
    users.erase(it);

    store->deleteUser(userId);

    return true;
}

int UserManager::generateUserId() {
    return nextUserId++;
}