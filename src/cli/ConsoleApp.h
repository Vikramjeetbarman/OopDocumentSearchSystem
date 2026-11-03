#pragma once
#include<vector>
#include<chrono>
#include "DocumentViewMode.h"
class UserManager;
class DocumentStore;
class InvertedIndex;
class Trie;
class SearchPipeline;
class DocumentService;

class SearchAnalytics;
class Document;
class SearchResult;
class Query;
class SearchEvent;

class ConsoleApp {
private:
    UserManager& userManager;
    DocumentStore& documentStore;
    InvertedIndex& invertedIndex;
    Trie& trie;
    SearchPipeline& searchPipeline;
   
    SearchAnalytics& searchAnalytics;

    std::string toLower(const std::string& s);
    std::string highlightText(const std::string& content,const std::string& queryText) ;
    std::string formatTime(std::chrono::system_clock::time_point tp);
    void showWelcomeMenu();
    void showUserMenu();
    void showAdminMenu();

    void handleRegister();
    void handleLogin();
    void handleLogout();
    void pauseScreen();
    std::string getPassword();

    void handlePublishDocument();
    void handleUnpublishDocument();
    void handlePublishDocument(int docId);
    void handleUnpublishDocument(int docId);
    void handleCreateDocument();
    void handleDeleteDocument();
    void handleDeleteDocument(int docId);
    void handleSearch();
    void handleDeleteUser();
    void handleDeleteOwnAccount();
    void handleMyDocuments();
    void handleEditDocument(Document* doc);
    void showDocument(Document* doc,DocumentViewMode mode);
    void printSearchResults(const std::vector<SearchResult>& results,const Query& query);
    void handleSearchHistory();
    void showSearchEvent(const SearchEvent& event);
    void handleSearchAnalytics();
    void rebuildSearchIndexes();

public:
   ConsoleApp(UserManager& userManager,
           DocumentStore& documentStore,
           InvertedIndex& invertedIndex,
           Trie& trie,
           SearchPipeline& searchPipeline,
           SearchAnalytics& searchAnalytics);

    void run();
};