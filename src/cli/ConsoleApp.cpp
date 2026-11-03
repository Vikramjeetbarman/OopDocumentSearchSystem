#include <iostream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <limits>
#include <string>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

#include "ConsoleApp.h"
#include "../user/Role.h"
#include "../user/UserManager.h"
#include "../documents/DocumentStore.h"
#include "../index/InvertedIndex.h"
#include "../search/SearchPipeline.h"
#include "../ranking/BM25Ranking.h"
#include "../ranking/PopularityRanking.h"
#include "../user/User.h"
#include "../index/Trie.h"
#include "../util/StringUtil.h"
#include "../observer/SearchAnalytics.h"
#include "../documents/Document.h"
#include "../ranking/RecentRanking.h"

const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* CYAN = "\033[36m";
const char* RESET = "\033[0m";

ConsoleApp::ConsoleApp(UserManager& userManager,
                       DocumentStore& documentStore,
                       InvertedIndex& invertedIndex,
                       Trie& trie,
                       SearchPipeline& searchPipeline,
                    SearchAnalytics& searchAnalytics)
    : userManager(userManager),
      documentStore(documentStore),
      invertedIndex(invertedIndex),
      trie(trie),
      searchPipeline(searchPipeline),
      searchAnalytics(searchAnalytics) {}



std::string ConsoleApp::getPassword() {

#ifdef _WIN32

    std::string password;
    char ch;

    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        }
        else {
            password += ch;
            std::cout << '*';
        }
    }

    std::cout << '\n';
    return password;

#else
    std::cin.clear();

    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }

    std::string password;
    std::getline(std::cin, password);
    return password;

#endif
}
std::string ConsoleApp::formatTime(
    std::chrono::system_clock::time_point tp)
{
    std::time_t t =
        std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;

    ss << std::put_time(
        std::localtime(&t),
        "%Y-%m-%d %H:%M");

    return ss.str();
}

void ConsoleApp::pauseScreen() {
    std::cout << "\nPress Enter to continue...";

    std::cin.clear();

    if (isatty(STDIN_FILENO)) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
}

std::string ConsoleApp::toLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

std::string ConsoleApp::highlightText(const std::string& content,const std::string& queryText) {

    std::unordered_set<std::string> queryWords;
    std::stringstream ss(queryText);
    std::string word;

    while (ss >> word) {
        queryWords.insert(toLower(word));
    }

    std::stringstream input(content);
    std::stringstream output;
    std::string token;

    while (input >> token) {

        std::string clean = toLower(token);

        if (queryWords.count(clean)) {
            output << "**" << token << "** ";
        } else {
            output << token << " ";
        }
    }

    return output.str();
}

void ConsoleApp::rebuildSearchIndexes() {
    invertedIndex.clear();
    trie.clear();

    for (const auto& doc : documentStore.getAllDocuments()) {
        invertedIndex.addDocument(doc);
        trie.insertDocument(doc);
    }
}

void ConsoleApp::run() {

    while (true) {

        User* current = userManager.getCurrentUser();

        if (!current) {
            showWelcomeMenu();
        }
        else if (current->isAdmin()) {
            showAdminMenu();
        }
        else {
            showUserMenu();
        }
    }
}

void ConsoleApp::showWelcomeMenu() {

    while (true) {

        // Console::clear();
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        std::cout << CYAN;
        std::cout << R"(

╔══════════════════════════════════════════════════════╗
║            MULTI-USER SEARCH ENGINE                  ║
╚══════════════════════════════════════════════════════╝

Welcome!

────────────────────────────────────────────────────────

  [1] Register
  [2] Login
  [3] Exit

────────────────────────────────────────────────────────

➜ Enter choice: )";

std::cout << RESET;

    int choice;

    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

        std::cout << RED << "\n✗ Please enter a valid number.\n" << RESET;

        // pauseScreen();
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        continue;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {

            case 1:
                handleRegister();

                if (userManager.getCurrentUser())
                    return;
                break;

            case 2:
                handleLogin();

                if (userManager.getCurrentUser())
                    return;

                break;

            case 3:
                exit(0);

            default:
                std::cout << RED<< "\n✗ Invalid choice. Please select 1-3.\n" << RESET;

                pauseScreen();
        }
    }
}

void ConsoleApp::handleLogout() {

    User* user = userManager.getCurrentUser();

    if (user) {
        searchPipeline.unregisterObserver(&user->getSearchHistory());
    }

    documentStore.flushToDisk();

    userManager.logout();

    std::cout << GREEN  << "\n✓ Logged out successfully.\n"  << RESET;
    
    pauseScreen();
}

void ConsoleApp::handleRegister() {

     #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    std::cout << CYAN;

    std::cout << R"(

    ┌────────────────────────────┐
    │          REGISTER          │
    └────────────────────────────┘

    ──────────────────────────────
    [0] Back
    ──────────────────────────────

)";

    std::cout << RESET;

    std::string username;
    std::string password;

    std::cout << "    Username : "<<std::flush;
    std::getline(std::cin, username);
    std::cin.clear();

    if(username == "0")
    return;

    std::cout << "    Password : "<<std::flush;
    password = getPassword();

    std::string confirmPassword;

    std::cout << "    Confirm Password : "<<std::flush;
    confirmPassword = getPassword();

    if (password != confirmPassword) {

        std::cout << RED
                << "\n✗ Passwords do not match.\n"
                << RESET;

        pauseScreen();
        return;
    }

    Role role = Role::USER;

    int newUserId = userManager.generateUserId();  

    bool success = userManager.createUser(newUserId, username, password, role);

    if (success){
        std::cout << GREEN << "\n✓ Registration successful.\n"  << RESET;
        pauseScreen();

        userManager.login(username, password);

        User* user = userManager.getCurrentUser();
        searchPipeline.registerObserver(&user->getSearchHistory());
    
    }
    else{
       std::cout << RED  << "\n✗ Registration failed.\n"  << "Username already exists.\n"  << RESET;

        pauseScreen();
    }
}

void ConsoleApp::handleLogin() {

   #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    std::cout << CYAN;

    std::cout << R"(

    ┌────────────────────────────┐
    │           LOGIN            │
    └────────────────────────────┘

    ──────────────────────────────
    [0] Back
    ──────────────────────────────

)";

    std::cout << RESET;

    std::string username;
    std::string password;

    std::cout << "    Username : "<<std::flush;
    std::getline(std::cin, username);
    std::cin.clear();

    if(username == "0") return;

    std::cout << "    Password : "<<std::flush;
    password = getPassword();

    bool success = userManager.login(username, password);

    if (success){

        User* user = userManager.getCurrentUser();
        searchPipeline.registerObserver(&user->getSearchHistory());
        std::cout << GREEN  << "\n✓ Login successful.\n"  << RESET;

        pauseScreen();
    }
    else{
        std::cout << RED  << "\n✗ Invalid username or password.\n"  << RESET;

        pauseScreen();
    }
}

void ConsoleApp::showUserMenu() {

    while (true) {

        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        std::cout << CYAN;

std::cout << R"(

╔══════════════════════════════════════════════════════╗
║                    USER DASHBOARD                    ║
╚══════════════════════════════════════════════════════╝

)";

std::cout << RESET;
std::cout << "Logged in as : "  << userManager.getCurrentUser()->getUserName()  << "\n";

std::cout << R"(

DOCUMENTS
────────────────────────────────────────────────────────

  [1] Create Document
  [2] Publish Document
  [3] Show My Documents
  [4] Unpublish Document
  [5] Delete Document

SEARCH
────────────────────────────────────────────────────────

  [6] Search
  [7] Search History

ACCOUNT
────────────────────────────────────────────────────────

  [8] Logout
  [9] Delete My Account

────────────────────────────────────────────────────────

➜ Enter choice: )";

        int choice;
        if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

        std::cout << RED
          << "\n✗ Please enter a valid number.\n"
          << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
            std::cin.get();

        continue;
        }
        
        switch (choice) {

            case 1:
                handleCreateDocument();
                break;

            case 2:
                handlePublishDocument();
                break;

            case 3:
                handleMyDocuments();
                break;

            case 4:
                handleUnpublishDocument();
                break;

            case 5:
                handleDeleteDocument();
                break;

            case 6:
                handleSearch();
                break;
            case 7:
                handleSearchHistory();
                break;
            case 8:
                handleLogout();
                return;
            case 9:
                handleDeleteOwnAccount();
                return;
            default:
                std::cout << RED
                        << "\n✗ Invalid choice. Please select 1-9.\n"
                        << RESET;

                pauseScreen();
        }
    }
}

void ConsoleApp::handlePublishDocument() {

    while(true){

    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    std::cout << CYAN;

    std::cout << R"(

    ┌────────────────────────────┐
    │      PUBLISH DOCUMENT      │
    └────────────────────────────┘

    [0] Back

    )";
    std::cout << RESET;

    int docId;

    std::cout << "Enter document ID to publish: ";
    
    if (!(std::cin >> docId)) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n');

        std::cout << RED
          << "\n✗ Please enter a valid number.\n"
          << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
            std::cin.get();

        continue;
        }

    if(docId == 0)
    return;

    auto docOpt = documentStore.getDocumentById(docId);

    if (!docOpt.has_value()) {
        std::cout << RED
          << "\n✗ Document not found.\n"
          << RESET;

        pauseScreen();
        continue;
    }

    Document* doc = docOpt.value();

    User* current = userManager.getCurrentUser();

    if (doc->getUserId() != current->getUserId()) {
        std::cout << RED
          << "\n✗ You can only publish your own documents.\n"
          << RESET;

        pauseScreen();
        continue;
    }
    if (doc->getVisibility() == Visibility::PUBLIC) {
        std::cout << GREEN
          << "\n✗ Document is already published.\n"
          << RESET;
        pauseScreen();
        return;
    }
    handlePublishDocument(docId);
    return;
}   
}

void ConsoleApp::handlePublishDocument(int docId)
{
   
    auto docOpt = documentStore.getDocumentById(docId);

    Document* doc = docOpt.value();

    doc->setVisibility(Visibility::PUBLIC);

    documentStore.updateDocument(*doc);

    searchPipeline.invalidateCache();

    std::cout << GREEN
          << "\n✓ Document published successfully.\n"
          << RESET;

        pauseScreen();
}

void ConsoleApp::handleUnpublishDocument() {
    while(true){

        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│     UNPUBLISH DOCUMENT     │
└────────────────────────────┘

[0] Back

)";

    std::cout << RESET;

    int docId;

    std::cout << "Enter document ID to unpublish: ";
    
    if (!(std::cin >> docId)) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n');

        std::cout << RED
          << "\n✗ Please enter a valid number.\n"
          << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
            std::cin.get();

        continue;
        }

     if (docId == 0)
            return;

    auto docOpt = documentStore.getDocumentById(docId);

    if (!docOpt.has_value()) {
        std::cout << RED
                      << "\n✗ Document not found.\n"
                      << RESET;

            pauseScreen();
            continue;
    }

    Document* doc = docOpt.value();
    User* current = userManager.getCurrentUser();

    if (doc->getUserId() != current->getUserId()) {
        std::cout << RED
                      << "\n✗ You can only unpublish your own documents.\n"
                      << RESET;

            pauseScreen();
            continue;
        
    }

    if (doc->getVisibility() == Visibility::PRIVATE) {
        std::cout << RED
                << "\n✗ Document is already private.\n"
                << RESET;

            pauseScreen();
            continue;
    }

    handleUnpublishDocument(docId);
    return;
}
}

void ConsoleApp::handleUnpublishDocument(int docId) {

    auto docOpt = documentStore.getDocumentById(docId);

    Document* doc = docOpt.value();
    
    doc->setVisibility(Visibility::PRIVATE);

    documentStore.updateDocument(*doc);

    searchPipeline.invalidateCache();

    std::cout << GREEN
                  << "\n✓ Document unpublished successfully.\n"
                  << RESET;
    pauseScreen();
}

void ConsoleApp::handleSearch() {

    while(true){

    #ifdef _WIN32
    system("cls");
    #else
        system("clear");
    #endif

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n');

    std::cout << CYAN;

    std::cout << R"(

    ┌────────────────────────────┐
    │          SEARCH            │
    └────────────────────────────┘

    [0] Back

)";

    std::cout << RESET;

    std::string queryText;

    std::cout << "Enter search query: ";
    std::getline(std::cin, queryText);

    if(queryText == "0")
    return;

    std::cout << R"(

Scope
────────────────────────────────────────

[1] Private
[2] Global
[3] Both

➜ Choice : )";

int scopeChoice;
std::cin >> scopeChoice;

    SearchScope scope = SearchScope::PRIVATE;

    if (scopeChoice == 2)
        scope = SearchScope::GLOBAL;
    else if (scopeChoice == 3)
        scope = SearchScope::BOTH;

    User* current = userManager.getCurrentUser();

    Query query(queryText, scope, current->getUserId());

    int searchMode=0;


while(searchMode != 1 &&searchMode != 2)
{
    std::cout << R"(

Search Mode
────────────────────────────────────────

[1] Standard Search
[2] Advanced Search

➜ Choice : )";

    std::cin >> searchMode;

    if(searchMode != 1 &&
       searchMode != 2)
    {
        std::cout << RED  << "\n✗ Invalid choice.\n"  << RESET;

        pauseScreen();
    }
}

        BM25Ranking bm25Ranking(invertedIndex,&documentStore);
        PopularityRanking popularityRanking(&documentStore);
        RecentRanking recentRanking(&documentStore);
        searchPipeline.setRankingStrategy(&bm25Ranking);

   
        if(searchMode == 2){

    char choice;

    std::cout
    << "\nApply Author Filter? (y/n) : ";

std::cin >> choice;

if(choice == 'y' ||choice == 'Y')
{
    int authorId;

    std::cout
        << "Author User ID : ";

    std::cin >> authorId;

    query.setAuthorFilter(authorId);
}

    std::cout
    << "\nApply Minimum Length Filter? (y/n) : ";

std::cin >> choice;

if(choice == 'y' ||choice == 'Y')
{
    size_t minLen;

    std::cout
        << "Minimum Length : ";

    std::cin >> minLen;

    query.setMinLengthFilter(minLen);
}


std::cout
<< "\nApply Date Filter? (y/n) : ";

std::cin >> choice;

if(choice == 'y' ||choice == 'Y')
{
    std::cout << R"(

Date Filter
────────────────────────────────────────

[0] All Time
[1] Last 24 Hours
[2] Last 7 Days
[3] Last 30 Days
[4] Last 90 Days
[5] Last 6 Months
[6] Last Year

➜ Choice : )";

    int dateChoice;
    std::cin >> dateChoice;

    auto now =
        std::chrono::system_clock::now();

    if(dateChoice != 0)
    {
        auto startTime = now;
        auto endTime = now;

        switch(dateChoice)
        {
            case 1:
                startTime = now - std::chrono::hours(24);
                break;

            case 2:
                startTime = now - std::chrono::hours(24 * 7);
                break;

            case 3:
                startTime = now - std::chrono::hours(24 * 30);
                break;

            case 4:
                startTime = now - std::chrono::hours(24 * 90);
                break;

            case 5:
                startTime = now - std::chrono::hours(24 * 180);
                break;

            case 6:
                startTime = now - std::chrono::hours(24 * 365);
                break;

            default:
                std::cout << RED  << "\n✗ Invalid choice.\n"  << RESET;

                pauseScreen();
                break;
        }

        query.setDateRangeFilter( startTime, endTime);
    }
}

std::cout << R"(

RANKING METHOD
────────────────────────────────────────

[1] Relevance (BM25)
[2] Popularity
[3] Most Recent

➜ Choice : )";

int rankChoice;
if (!(std::cin >> rankChoice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

        std::cout << RED  << "\n✗ Please enter a valid number.\n"  << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
            std::cin.get();

        continue;
        }

switch(rankChoice)
{
case 2:
    searchPipeline.setRankingStrategy(&popularityRanking);
    break;

case 3:
    searchPipeline.setRankingStrategy(&recentRanking);
    break;

default:
    searchPipeline.setRankingStrategy( &bm25Ranking);
}
}
    
    std::vector<SearchResult> results = searchPipeline.execute(query);

    if(results.empty())
{
    std::cout << GREEN  << "\nNo matching documents found.\n"  << RESET;

    pauseScreen();
    continue;
}

    while(true)
{
    printSearchResults(results, query);

    std::cout << R"(

──────────────────────────────
[0] Back
──────────────────────────────

➜ Select document : )";

    int selected;
    std::cin >> selected;

    if(selected == 0)
        break;

    if(selected < 1 ||
       selected > static_cast<int>(results.size()))
    {
        std::cout << "Invalid selection\n";
        continue;
    }

    int docId = results[selected - 1].getDocumentId();

    auto docOpt = documentStore.getDocumentById(docId);

    if(!docOpt.has_value())
        continue;

    Document* doc = docOpt.value();

    doc->recordImpression();

    showDocument(doc,DocumentViewMode::VIEW_ONLY);
}
    }
}

void ConsoleApp::handleCreateDocument() {

    #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif


    std::cout << CYAN;

    std::cout << R"(

    ┌────────────────────────────┐
    │      CREATE DOCUMENT       │
    └────────────────────────────┘

    [0] Back

    )";

std::cout << RESET;

    std::string title;

    std::cin.ignore();

    std::cout << "Enter title  : ";
    std::getline(std::cin, title);

    if(title == "0")
    return;

    std::string content;
    std::string line;

        std::cout << R"(

────────────────────────────────────────
Enter content.
Type END on a new line to finish.
────────────────────────────────────────

)";

    while (true) {

        std::getline(std::cin, line);

        if (line == "END")
            break;

        content += line + '\n';
    }

    User* current = userManager.getCurrentUser();

    int docId=documentStore.generateDocId();  

    Document doc(
        docId,
        current->getUserId(),
        title,
        content,
        Visibility::PRIVATE,
        std::chrono::system_clock::now()
    );

    documentStore.addDocument(doc);

    rebuildSearchIndexes();
    searchPipeline.invalidateCache();

    std::cout << GREEN  << "\n✓ Document created successfully.\n"  << RESET;

    std::cout << "Document ID : " << docId << "\n";

     std::cin.putback('\n');

    pauseScreen();
}

void ConsoleApp::handleDeleteDocument() {

    while(true){

    #ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << CYAN;

        std::cout << R"(

    ┌────────────────────────────┐
    │      DELETE DOCUMENT       │
    └────────────────────────────┘

    [0] Back

)";

    std::cout << RESET;
    int docId;

    std::cout << "Enter document ID to delete: ";
    

    if (!(std::cin >> docId)) {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n');

        std::cout << RED  << "\n✗ Please enter a valid number.\n"  << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
        std::cin.get();

        continue;
        }

    if(docId==0)return;

    auto docOpt = documentStore.getDocumentById(docId);

    if (!docOpt.has_value()) {
        std::cout << RED  << "\n✗ Document not found.\n"  << RESET;

            pauseScreen();
            continue;
    }

    Document* doc = docOpt.value();
    User* current = userManager.getCurrentUser();

    std::cout << "\nDocument : "  << doc->getTitle()  << '\n';

    if (!current->isAdmin() && doc->getUserId() != current->getUserId()) {
        std::cout << RED  << "\n✗ You do not have permission to delete this document.\n"  << RESET;

            pauseScreen();
            continue;
    }
    
    handleDeleteDocument(docId);
}
}

void ConsoleApp::handleDeleteDocument(int docId)
{
        
       std::string confirmation;

        std::cout << RED  << "\n⚠ This action cannot be undone.\n"  << RESET;

        std::cout << "Type DELETE to confirm : ";
        std::cin >> confirmation;

        if (confirmation != "DELETE") {

            std::cout << GREEN
                      << "\nDeletion cancelled.\n"
                      << RESET;

            pauseScreen();
            return;
        }
    invertedIndex.removeDocument(docId);
    documentStore.removeDocument(docId);
    searchPipeline.invalidateCache();
    std::cout << GREEN  << "\n✓ Document deleted successfully.\n"  << RESET;

    pauseScreen();

}

void ConsoleApp::handleDeleteUser() {

    while (true) {

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│        DELETE USER         │
└────────────────────────────┘

[0] Back

)";

    std::cout << RESET;
    int userId;

    std::cout << "Enter user ID to delete: ";
    

    if (!(std::cin >> userId)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

        std::cout << RED  << "\n✗ Please enter a valid number.\n"  << RESET;

        // pauseScreen();

        std::cout << "\nPress Enter to continue...";
            std::cin.get();

        continue;
        }

    if (userId == 0)
            return;

    if (!userManager.userExists(userId)) {
        std::cout << RED  << "\n✗ User not found.\n"  << RESET;

            pauseScreen();
            continue;
    }

    std::cout << "\nUser : "
                  << userManager.getUser(userId)->getUserName()
                  << '\n';

        std::cout << GREEN
                  << "\n⚠ This will permanently delete:\n"
                  << "  • User account\n"
                  << "  • All owned documents\n"
                  << "  • Published documents\n"
                  << "\nThis action cannot be undone.\n"
                  << RESET;

        std::string confirmation;

        std::cout << "\nType DELETE to confirm : ";
        std::getline(std::cin >> std::ws, confirmation);

        if (confirmation != "DELETE") {

            std::cout << GREEN  << "\nDeletion cancelled.\n"  << RESET;

            pauseScreen();
            continue;
        }

    std::vector<Document> allDocs = documentStore.getAllDocuments();

    for (auto& doc : allDocs) {

        if (doc.getUserId() == userId) {

            if (doc.getVisibility() == Visibility::PUBLIC) {
                invertedIndex.removeDocument(doc.getId());
            }

            documentStore.removeDocument(doc.getId());
        }
    }

    userManager.deleteUser(userId);

    documentStore.flushToDisk();

    searchPipeline.invalidateCache();

    std::cout << GREEN  << "\n✓ User and all associated documents deleted successfully.\n"  << RESET;

        pauseScreen();
    return;
}
}

void ConsoleApp::showAdminMenu() {

    while (true) {

        #ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        User* user = userManager.getCurrentUser();

        std::cout << CYAN;

        std::cout << R"(

╔══════════════════════════════════════════════════════╗
║                   ADMIN DASHBOARD                    ║
╚══════════════════════════════════════════════════════╝

)";

        std::cout << RESET;

        std::cout << "Logged in as : "
                  << user->getUserName()
                  << " (ADMIN)\n";

        std::cout << R"(

DOCUMENTS
────────────────────────────────────────────────────────

  [1] Create Document
  [2] Publish Document
  [3] My Documents
  [4] Unpublish Document
  [5] Delete Document

SEARCH
────────────────────────────────────────────────────────

  [6] Search
  [7] Search History

ADMIN
────────────────────────────────────────────────────────

  [8] Delete User
  [9] Search Analytics

ACCOUNT
────────────────────────────────────────────────────────

  [10] Logout

────────────────────────────────────────────────────────

➜ Enter choice: )";


        int choice;
        if (!(std::cin >> choice)) {

            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            std::cout << RED  << "\n✗ Please enter a valid number.\n"  << RESET;

            // pauseScreen();

            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {

            case 1:
                handleCreateDocument();
                break;

            case 2:
                handlePublishDocument();
                break;

            case 3:
                handleMyDocuments();
                break;

            case 4:
                handleUnpublishDocument();
                break;

            case 5:
                handleDeleteDocument();
                break;

            case 6:
                handleSearch();
                break;

            case 7:
                handleSearchHistory();
                break;

            case 8:
                handleDeleteUser();
                break;

            case 9:
                handleSearchAnalytics();
                break;

            case 10:
                handleLogout();
                return;

            default:
                std::cout << RED  << "\n✗ Invalid choice. Please select 1-10.\n"  << RESET;

                pauseScreen();
        }
    }
}

void ConsoleApp::handleDeleteOwnAccount() {

     while (true) {

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        User* current = userManager.getCurrentUser();

        if (!current) {

            std::cout << RED  << "\n✗ No user logged in.\n"  << RESET;

            pauseScreen();
            return;
        }

        std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│       DELETE ACCOUNT       │
└────────────────────────────┘

[0] Back

)";

        std::cout << RESET;

        std::cout << "Username : "  << current->getUserName()  << "\n";

        std::cout << RED
                  << "\n⚠ This will permanently delete:\n"
                  << "  • Your account\n"
                  << "  • All private documents\n"
                  << "  • All published documents\n"
                  << "  • Search history\n"
                  << "\nThis action cannot be undone.\n"
                  << RESET;

        std::string confirmation;

        std::cout
            << "\nType DELETE MY ACCOUNT to confirm : ";

        std::getline(std::cin >> std::ws, confirmation);

        if (confirmation == "0")
            return;

        if (confirmation != "DELETE MY ACCOUNT") {

            std::cout << RED  << "\nDeletion cancelled.\n"  << RESET;

            pauseScreen();
            continue;
        }

    int userId = current->getUserId();

    std::vector<Document> allDocs = documentStore.getAllDocuments();

    for (const auto& doc : allDocs) {

        if (doc.getUserId() == userId) {

            if (doc.getVisibility() == Visibility::PUBLIC) {
                invertedIndex.removeDocument(doc.getId());
            }

            documentStore.removeDocument(doc.getId());
        }
    }


    userManager.deleteUser(userId);

    documentStore.flushToDisk();

    searchPipeline.invalidateCache();

    std::cout << GREEN  << "\n✓ Account and all associated documents deleted successfully.\n"  << RESET;

    userManager.logout();

    //pauseScreen();
    std::cout << "\nPress Enter to continue...";
    std::cin.get();

    return;
}
}

void ConsoleApp::handleMyDocuments()
{
    User* currentUser =
        userManager.getCurrentUser();

    
    while(true)
    {
       #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        auto docs =
        documentStore.getDocumentsByUser(
            currentUser->getUserId());

    if(docs.empty())
    {
        std::cout << GREEN  << "\nNo documents found.\n"  << RESET;

            // pauseScreen();
             std::cout << "\nPress Enter to continue...";
            std::cin.get();
            return;
    }

         std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│        MY DOCUMENTS        │
└────────────────────────────┘

)";

        std::cout << RESET;

        std::cout << "Total Documents : " << docs.size() << "\n\n";


        for(size_t i = 0; i < docs.size(); i++)
        {
             std::cout << "[" << i + 1 << "] " << docs[i]->getTitle();


            if(docs[i]->getVisibility()== Visibility::PUBLIC)
            {
                std::cout << " (PUBLIC)";
            }
            else
            {
                std::cout << " (PRIVATE)";
            }

            std::cout << "\n";
        }

         std::cout << R"(

──────────────────────────────
[0] Back
──────────────────────────────

➜ Select document : )";


        int choice;
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            std::cout << RED  << "\n✗ Please enter a valid number.\n"  << RESET;

            pauseScreen();
            continue;
        }

        if(choice == 0)
            return;

        if(choice < 1 ||
           choice > static_cast<int>(docs.size()))
        {
            std::cout << RED  << "\n✗ Invalid selection.\n"  << RESET;

            pauseScreen();
            continue;
        }

        Document* doc = docs[choice - 1];

        doc->recordImpression();

        showDocument(doc,DocumentViewMode::MANAGE);
    }
}
void ConsoleApp::handleEditDocument(Document* doc) {

    #ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

    std::cout << CYAN;

    std::cout << R"(

┌────────────────────────────┐
│       EDIT DOCUMENT        │
└────────────────────────────┘

Type 0 as the title to cancel.

)";

    std::cout << RESET;
    std::string newTitle;
    std::string newContent;

    std::cout << "Current Title : "  << doc->getTitle()  << "\n";

    std::cout << "New Title : ";
    std::getline(std::cin, newTitle);

    if (newTitle == "0")
        return;

    if (newTitle.empty()) {

        std::cout << RED  << "\n✗ Title cannot be empty.\n"  << RESET;

        pauseScreen();
        return;
    }

    std::cout << R"(

────────────────────────────────────────
Enter content.
Type END on a new line to finish.
────────────────────────────────────────

)";

std::string line;

    while (true) {

        std::getline(std::cin, line);

        if (line == "END")
            break;

        newContent += line;
        newContent += '\n';
    }

    if (newContent.empty()) {

        std::cout << RED  << "\n✗ Content cannot be empty.\n"  << RESET;

        pauseScreen();
        return;
    }


    doc->setTitle(newTitle);
    doc->setContent(newContent);

    documentStore.updateDocument(*doc);

    invertedIndex.removeDocument(doc->getId());

    invertedIndex.addDocument(*doc);

    trie.insertDocument(*doc);



searchPipeline.invalidateCache();

     std::cout << GREEN
              << "\n✓ Document updated successfully.\n"
              << RESET;


     std::cin.putback('\n');
    pauseScreen();


    }

void ConsoleApp::showDocument(Document* doc,DocumentViewMode mode)
{
    
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    User* author =
        userManager.getUser(doc->getUserId());

    std::cout << CYAN;

    std::cout << R"(

┌────────────────────────────┐
│         DOCUMENT           │
└────────────────────────────┘

)";

    std::cout << RESET;
    std::cout << "Title      : "  << doc->getTitle()  << '\n';

    std::cout << "Author     : ";

    if(author)
        std::cout << author->getUserName();
    else
        std::cout << "Unknown";

    std::cout << '\n';

    std::cout << "Visibility : ";

    if(doc->getVisibility() == Visibility::PUBLIC)
    {
        std::cout << "PUBLIC";
    }
    else
    {
        std::cout << "PRIVATE";
    }

    std::cout << "Created At : " << formatTime(doc->getCreatedAt()) << '\n';

    std::cout << R"(

────────────────────────────────────────

)";

    std::cout
        << doc->getContent();

    std::cout << R"(

────────────────────────────────────────

)";

    if(mode == DocumentViewMode::VIEW_ONLY)
{
    while(true)
        {
            std::cout << R"(

[0] Back

➜ Choice : )";

            int choice;

            if(!(std::cin >> choice))
            {
                std::cin.clear();

                std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << RED  << "\n✗ Invalid input.\n"  << RESET;

                pauseScreen();
                continue;
            }

            if(choice == 0)
                return;

            std::cout << RED << "\n✗ Invalid choice.\n" << RESET;

            pauseScreen();
        }
}

    while(true)
    {
        std::cout << "[0] Back\n";
        std::cout << "[1] Edit\n";
        std::cout << "[2] Delete\n";

        if(doc->getVisibility()
            == Visibility::PRIVATE)
        {
            std::cout << "[3] Publish\n";
        }
        else
        {
            std::cout << "[3] Make Private\n";
        }

        std::cout
            << "\n➜ Choice : ";

        int choice;

        if(!(std::cin >> choice))
        {
            std::cin.clear();

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

            std::cout << RED << "\n✗ Invalid input.\n" << RESET;

            pauseScreen();
            continue;
        }

        switch(choice)
        {
            case 0:
                return;

            case 1:
                handleEditDocument(doc);

                showDocument(doc,mode);

                return;

            case 2:
                handleDeleteDocument(doc->getId());

                return;

            case 3:

                if(doc->getVisibility()== Visibility::PRIVATE)
                {
                    handlePublishDocument(doc->getId());
                }
                else
                {
                    handleUnpublishDocument(doc->getId());
                }

                showDocument(doc,mode);

                return;

            default:

                std::cout << RED  << "\n✗ Invalid choice.\n"  << RESET;

                pauseScreen();
        }
    }
}

void ConsoleApp::printSearchResults(
    const std::vector<SearchResult>& results,
    const Query& query)
{
    std::cout << CYAN;

    std::cout << R"(

┌────────────────────────────┐
│       SEARCH RESULTS       │
└────────────────────────────┘

)";

    std::cout << RESET;

    std::cout
        << "Query : "
        << query.getQueryText()
        << "\n";

    std::cout
        << "Results Found : "
        << results.size()
        << "\n\n";

    for(size_t i = 0; i < results.size(); i++)
    {
        int docId =
            results[i].getDocumentId();

        auto docOpt = documentStore.getDocumentById(docId);

        if(!docOpt.has_value())
            continue;

        const Document* doc =docOpt.value();

        User* author =userManager.getUser(doc->getUserId());

        std::string preview = doc->getContent().substr(0,80);

        std::string highlighted = highlightText( preview, query.getQueryText());

         std::cout
            << "[" << i + 1 << "] "
            << doc->getTitle();

        std::cout << "\n";

         std::cout << "    Author : ";

        if(author)
            std::cout<< author->getUserName();
        else
            std::cout << "Unknown";

        std::cout
        << "    Created : "
        << formatTime(doc->getCreatedAt())
        << '\n';

        std::cout
            << "   "
            << highlighted
            << "...\n\n";
    }

}

void ConsoleApp::handleSearchHistory()
{
    User* currentUser =
        userManager.getCurrentUser();

    const auto& history =
        currentUser
            ->getSearchHistory()
            .getHistory();

    if(history.empty())
    {
        std::cout << GREEN << "\nNo search history found.\n" << RESET;

        pauseScreen();
        return;
    }

    while(true)
    {
        #ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│       SEARCH HISTORY       │
└────────────────────────────┘

)";

        std::cout << RESET;

        std::cout
            << "Total Searches : "
            << history.size()
            << "\n\n";


        int displayIndex = 1;

        for(int i = history.size() - 1;
            i >= 0;
            i--)
        {
             std::cout
                << "[" << displayIndex++ << "] "
                << history[i].getQueryText()
                << '\n';
        }

        std::cout << R"(

──────────────────────────────
[0] Back
──────────────────────────────

➜ Select search : )";

        int choice;
        if(!(std::cin >> choice))
        {
            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            std::cout << RED << "\n✗ Please enter a valid number.\n" << RESET;

            pauseScreen();
            continue;
        }

        if(choice == 0)
            return;

        if(choice < 1 ||
           choice > static_cast<int>(history.size()))
        {
            std::cout << RED << "\n✗ Invalid selection.\n" << RESET;

            pauseScreen();
            continue;
        }

        int historyIndex = history.size() - choice;

        showSearchEvent( history[historyIndex]);
    }
}

void ConsoleApp::showSearchEvent(const SearchEvent& event)
{
    const auto& results =
        event.getResults();

    while(true)
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << CYAN;

        std::cout << R"(

┌────────────────────────────┐
│       SEARCH RESULTS       │
└────────────────────────────┘

)";

        std::cout << RESET;

        std::cout
            << "Query : "
            << event.getQueryText()
            << "\n";

        std::cout
            << "Results Found : "
            << results.size()
            << "\n\n";

        for(size_t i = 0;
            i < results.size();
            i++)
        {
            int docId =
                results[i].getDocumentId();

            auto docOpt =
                documentStore
                    .getDocumentById(docId);

            if(!docOpt.has_value())
                continue;

            Document* doc =
                docOpt.value();

            std::cout
                << "[" << i + 1 << "] "
                << doc->getTitle();

            if(doc->getVisibility()
                == Visibility::PUBLIC)
            {
                std::cout
                    << " (PUBLIC)";
            }
            else
            {
                std::cout
                    << " (PRIVATE)";
            }

            std::cout << '\n';
        }

        std::cout << R"(

──────────────────────────────
[0] Back
──────────────────────────────

➜ Select document : )";

        int choice;

        if(!(std::cin >> choice))
        {
            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            std::cout << RED
                      << "\n✗ Please enter a valid number.\n"
                      << RESET;

            pauseScreen();

            continue;
        }

        if(choice == 0)
            return;

        if(choice < 1 ||
           choice > static_cast<int>(results.size()))
        {
            std::cout << RED
                      << "\n✗ Invalid selection.\n"
                      << RESET;

            pauseScreen();

            continue;
        }

        int docId =
            results[choice - 1]
                .getDocumentId();

        auto docOpt =
            documentStore
                .getDocumentById(docId);

        if(!docOpt.has_value())
        {
            std::cout << RED
                      << "\n✗ Document no longer exists.\n"
                      << RESET;

            pauseScreen();

            continue;
        }

        showDocument(
            docOpt.value(),
            DocumentViewMode::VIEW_ONLY);
    }
}
void ConsoleApp::handleSearchAnalytics()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    User* currentUser =
        userManager.getCurrentUser();

    if(currentUser->getRole() != Role::ADMIN)
    {
        std::cout << RED
                  << "\n✗ Access denied.\n"
                  << RESET;

        pauseScreen();
        return;
    }

    std::cout << CYAN;

    std::cout << R"(

┌────────────────────────────┐
│     SEARCH ANALYTICS       │
└────────────────────────────┘

)";

    std::cout << RESET;

    std::cout
        << "Total Searches : "
        << searchAnalytics.getTotalSearches()
        << "\n\n";

    const auto& frequencies =
        searchAnalytics.getQueryFrequency();

    if(frequencies.empty())
    {
        std::cout << GREEN
                  << "No searches recorded yet.\n"
                  << RESET;

        pauseScreen();
        return;
    }

    std::vector<
        std::pair<std::string,int>>
        queries(
            frequencies.begin(),
            frequencies.end());

    std::sort(
        queries.begin(),
        queries.end(),
        [](const auto& a,
           const auto& b)
        {
            return a.second > b.second;
        });

    std::cout
        << "Most Popular Query\n";

    std::cout
        << "────────────────────────────────────────\n";

    std::cout
        << queries.front().first
        << " ("
        << queries.front().second
        << " searches)\n\n";

    std::cout
        << "Top Queries\n";

    std::cout
        << "────────────────────────────────────────\n\n";

    int rank = 1;

    for(const auto& query : queries)
    {
        std::cout
            << "[" << rank++ << "] "
            << query.first
            << " ("
            << query.second
            << " searches)\n";

        if(rank > 10)
            break;
    }

    pauseScreen();
}