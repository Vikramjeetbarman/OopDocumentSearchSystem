#pragma once

#include "RankingStrategy.h"

class DocumentStore;

class RecentRanking : public RankingStrategy
{
private:
    DocumentStore* documentStore;

public:
    explicit RecentRanking(
        DocumentStore* documentStore);

    std::vector<SearchResult> rank(
        const Query&,
        const std::vector<SearchResult>& results
    ) override;

    const char* getName() const override;
};