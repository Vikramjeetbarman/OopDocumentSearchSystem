#pragma once

#include "../core/SearchResult.h"
#include "SearchContext.h"
#include<vector>

class Query;
class Document;
/*
 * Strategy interface defining the contract for all search behaviors.
 * Concrete implementations (Exact, Phrase, Prefix, etc.) must implement this interface.
 */
class SearchStrategy{

public:    

    /*
     * Executes a search based on the given query.
     * This is the core operation implemented by all strategies.
     */
    virtual std::vector <SearchResult> search(const Query& query,const std::vector<Document>& documents,const SearchContext& context)=0;
    virtual MatchType getMatchType() const = 0;
    virtual ~SearchStrategy()=default;
};

