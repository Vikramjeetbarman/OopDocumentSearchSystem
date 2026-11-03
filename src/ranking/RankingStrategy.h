#pragma once

#include <vector>

class Query;
class SearchResult;

class RankingStrategy {
public:
    virtual std::vector<SearchResult>rank(const Query& query,const std::vector<SearchResult>& results) = 0;

    virtual const char* getName() const=0;

    virtual ~RankingStrategy() = default;
};
