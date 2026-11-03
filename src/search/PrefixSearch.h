#pragma once
#include "SearchStrategy.h"
#include <vector>

class Document;
class InvertedIndex;

class PrefixSearch :public SearchStrategy{

public:
    PrefixSearch();

    

    std::vector <SearchResult> search(const Query& query,const std::vector<Document>& documents,const SearchContext& context) override;

   
    MatchType getMatchType() const override
    {
        return MatchType::PREFIX;
    }
    
};