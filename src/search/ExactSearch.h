#pragma once
#include "SearchStrategy.h"
#include <vector>

class Document;
class InvertedIndex;

class ExactSearch :public SearchStrategy{
private:


public:
    //ExactSearch(const std::vector<Document>& documents);
    ExactSearch();

   

    std::vector <SearchResult> search(const Query& query,const std::vector<Document>& documents,const SearchContext& context) override;

    
    MatchType getMatchType() const override
    {
        return MatchType::EXACT;
    }
    

};