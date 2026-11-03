#pragma once
#include "SearchStrategy.h"

class InvertedIndex;

class FuzzySearch : public SearchStrategy {
private:
   
    [[maybe_unused]] int threshold = 1;

public:
    explicit FuzzySearch();

    std::vector<SearchResult> search(const Query& query,const std::vector<Document>&,const SearchContext& context) override;

   MatchType getMatchType() const override
    {
        return MatchType::FUZZY;
    }
};