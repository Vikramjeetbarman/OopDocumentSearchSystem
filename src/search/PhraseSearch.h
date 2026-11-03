#pragma once
#include "SearchStrategy.h"
#include <vector>

class Document;
class InvertedIndex;

class PhraseSearch :public SearchStrategy{

public:

    //ExactSearch(const std::vector<Document>& documents);
    PhraseSearch();

   
     std::string normalizeText(const std::string&text);


    std::vector <SearchResult> search(const Query& query,const std::vector<Document>& documents,const SearchContext& context) override;

    MatchType getMatchType() const override
    {
        return MatchType::PHRASE;
    }

   
};