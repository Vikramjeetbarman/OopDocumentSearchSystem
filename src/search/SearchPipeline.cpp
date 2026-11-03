#include<algorithm>
# include "SearchPipeline.h"
#include "SearchStrategy.h"
#include "../core/Query.h"
#include "../core/SearchResult.h"
#include "../documents/Document.h"
#include "../documents/DocumentStore.h"
#include "../ranking/RankingStrategy.h"
#include "SearchContext.h"
#include "../observer/SearchEvent.h"
#include "../observer/SearchObserver.h"
#include "../filter/FilterChain.h"
#include "../filter/AuthorFilter.h"
#include "../filter/MinLengthFilter.h"
#include "../filter/DateFilter.h"
#include "../security/AccessPolicy.h"


SearchPipeline::SearchPipeline( RankingStrategy* rankingStrategy,DocumentStore* documentStore,InvertedIndex& index,Trie* trie,AccessPolicy* accessPolicy,size_t cacheCapacity)
:rankingStrategy(rankingStrategy),documentStore(documentStore),index(index),trie(trie),accessPolicy(accessPolicy),cache(cacheCapacity){
    searchStrategies.push_back(std::make_unique<PhraseSearch>());
    searchStrategies.push_back(std::make_unique<ExactSearch>());
    searchStrategies.push_back(std::make_unique<PrefixSearch>());
    searchStrategies.push_back(std::make_unique<FuzzySearch>());
}

int SearchPipeline::getPriority(MatchType type)
{
    switch(type)
    {
        case MatchType::PHRASE: return 4;
        case MatchType::EXACT:  return 3;
        case MatchType::PREFIX: return 2;
        case MatchType::FUZZY:  return 1;
    }

    return 0;
}
    
void SearchPipeline::registerObserver(SearchObserver* observer) {
    observers.push_back(observer);
}

void SearchPipeline::unregisterObserver(SearchObserver* observer) {

    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void SearchPipeline::invalidateCache() {
    cache.clear();
}

void SearchPipeline::setRankingStrategy(RankingStrategy* strategy) {
    rankingStrategy = strategy;
}

std::vector<SearchResult> SearchPipeline::execute(const Query& query){

    std::string author = "";
if (query.getAuthorFilter().has_value()) {
    author = std::to_string(*query.getAuthorFilter());
}

size_t minLen = query.getMinLengthFilter().has_value()
    ? *query.getMinLengthFilter()
    : static_cast<size_t>(-1);

std::string dateRange = "";
if (query.getDateRangeFilter()) {
    auto range = *query.getDateRangeFilter();

    auto start = std::chrono::duration_cast<std::chrono::milliseconds>(
                     range.first.time_since_epoch()).count();

    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(
                   range.second.time_since_epoch()).count();

    dateRange = std::to_string(start) + "_" + std::to_string(end);
}
    
CacheKey key(
    query.getQueryText(),
    rankingStrategy->getName(),
    query.getSearchScope(),
    query.getUserId(),
    author,
    minLen,
    dateRange
);
    if (cache.contains(key)) {
        auto cachedResults = cache.get(key);

    SearchEvent event(
        query.getUserId(),
        query.getQueryText(),
        cachedResults
    );

    for (SearchObserver* observer : observers) {
        observer->onSearchExecuted(event);
    }

    return cachedResults;
    }


    int userId=query.getUserId();
    std::vector<Document> allDocuments=documentStore->getAllDocuments();

    std::vector<Document> documents =accessPolicy->filter(allDocuments, query);

    FilterChain chain;


if (query.getAuthorFilter()) {
    chain.addFilter(std::make_unique<AuthorFilter>(*query.getAuthorFilter()));
}

if (query.getMinLengthFilter()) {
    chain.addFilter(std::make_unique<MinLengthFilter>(*query.getMinLengthFilter()));
}

if (query.getDateRangeFilter()) {
    auto range = *query.getDateRangeFilter();

    chain.addFilter(std::make_unique<DateFilter>(range.first,range.second));
}

documents = chain.apply(documents);

    SearchContext context(index,trie);

    std::unordered_map<int,SearchResult>merged;

    auto addResults = [&](const std::vector<SearchResult>& res, MatchType type) {

    for (const auto&result : res) {

        auto r=result;

        int id = r.getDocumentId();
        r.setMatchType(type);

        auto it = merged.find(id);

        if (it == merged.end()) {
            merged.emplace(id, r);
        } else {
            MatchType existing = it->second.getMatchType();

            if(getPriority(type) >
        getPriority(existing))
        {
            it->second = r;
        }
        }
    }
    };

    // addResults(phraseSearch.search(query, documents, context), MatchType::PHRASE);
    // addResults(exactSearch.search(query, documents, context), MatchType::EXACT);
    // addResults(prefixSearch.search(query, documents, context), MatchType::PREFIX);
    // addResults(fuzzySearch.search(query, documents, context), MatchType::FUZZY);
    for (const auto& strategy : searchStrategies)
    {
        addResults(
            strategy->search(query, documents, context),
            strategy->getMatchType()
        );
    }

    std::vector<SearchResult> searchResults;

    for(auto& [id ,r]:merged){
        searchResults.push_back(r);
    }

    if (rankingStrategy) {
        searchResults = rankingStrategy->rank(query, searchResults);
    }

    

    SearchEvent event(userId,query.getQueryText(),searchResults);

    for (SearchObserver* observer : observers) {
        observer->onSearchExecuted(event);
    }

    cache.put(key, searchResults);
    return searchResults;
}