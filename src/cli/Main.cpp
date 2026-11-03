#include <iostream>
#include <filesystem>

#include "ConsoleApp.h"

#include "../user/UserManager.h"
#include "../user/FileUserStore.h"

#include "../documents/DocumentStore.h"
#include "../storage/FileStorage.h"

#include "../index/InvertedIndex.h"
#include "../index/Trie.h"

#include "../ranking/BM25Ranking.h"
#include "../ranking/PopularityRanking.h"

#include "../search/SearchPipeline.h"
#include "../security/DefaultAccessPolicy.h"

#include "../user/SearchHistory.h"
#include "../observer/SearchAnalytics.h"

namespace {

std::filesystem::path resolveDataPath(const std::string& relativePath) {
    std::filesystem::path input(relativePath);

    if (input.is_absolute()) {
        return input;
    }

    std::filesystem::path current = std::filesystem::current_path();

    for (std::filesystem::path dir = current; ; dir = dir.parent_path()) {
        std::filesystem::path candidate = dir / input;

        if (std::filesystem::exists(candidate) && std::filesystem::is_regular_file(candidate)) {
            return candidate;
        }

        if (dir == dir.parent_path()) {
            break;
        }
    }

    return std::filesystem::absolute(input);
}

} // namespace

int main() {

    std::cout << "===== Mini Search Engine Starting =====\n";

    const auto usersPath = resolveDataPath("data/users.txt");
    const auto documentsPath = resolveDataPath("data/documents.txt");

    FileUserStore userStore(usersPath.string());
    FileStorage documentStore(documentsPath.string());
     
    UserManager userManager(&userStore);

    InvertedIndex index;
    Trie trie;

    auto docs = documentStore.getAllDocuments();

    for (const Document& doc : docs) {
        index.addDocument(doc);
        trie.insertDocument(doc);
    }

    DefaultAccessPolicy accessPolicy;

    BM25Ranking bm25Ranking(index,&documentStore);
    PopularityRanking popularityRanking(&documentStore);

    SearchPipeline searchPipeline(
        &bm25Ranking,        
        &documentStore,
        index,
        &trie,
        &accessPolicy,
        100                  
    );

    SearchHistory history;
    searchPipeline.registerObserver(&history);

    SearchAnalytics searchAnalytics;

    searchPipeline.registerObserver(
        &searchAnalytics);

    ConsoleApp app(
        userManager,
        documentStore,
        index,
        trie,
        searchPipeline,
        searchAnalytics
    );

    app.run();

    std::cout << "Shutting down...\n";

    return 0;
}