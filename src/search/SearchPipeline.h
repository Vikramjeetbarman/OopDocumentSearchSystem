#pragma once

#include <vector>
#include "../cache/LRUCache.h"
#include "../cache/CacheKey.h"
#include "../security/AccessPolicy.h"
#include "ExactSearch.h"
#include "FuzzySearch.h"
#include "PhraseSearch.h"
#include "PrefixSearch.h"
#include <memory>

class Query;
class SearchResult;

class DocumentStore;
class RankingStrategy;
class Trie;
class InvertedIndex;
class SearchObserver;




class SearchPipeline
{
private:
    
    RankingStrategy* rankingStrategy;
    DocumentStore* documentStore;
    InvertedIndex& index;
    Trie* trie;
    std::vector<SearchObserver*> observers;
    AccessPolicy* accessPolicy;
    LRUCache<CacheKey, std::vector<SearchResult>, CacheKeyHash> cache;
    
    std::vector<std::unique_ptr<SearchStrategy>> searchStrategies;
    // PhraseSearch phraseSearch;
    // ExactSearch exactSearch;
    // PrefixSearch prefixSearch;
    // FuzzySearch fuzzySearch;

public:
    SearchPipeline(RankingStrategy* rankingStrategy,DocumentStore* documentStore,InvertedIndex& index,Trie* trie,AccessPolicy* accessPolicy,size_t cacheCapacity);

    void registerObserver(SearchObserver* observer);
    void unregisterObserver(SearchObserver* observer);
    void setRankingStrategy(RankingStrategy* strategy);
    std::vector<SearchResult> execute(const Query& query);
    static int getPriority(MatchType type);

    void invalidateCache();
};
