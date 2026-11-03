#pragma once
#include "RankingStrategy.h"

class DocumentStore;

class PopularityRanking : public RankingStrategy {
private:
    DocumentStore* documentStore;

public:
    explicit PopularityRanking(DocumentStore* store);

    std::vector<SearchResult> rank(const Query&,const std::vector<SearchResult>& results) override;

    const char* getName() const override;

};