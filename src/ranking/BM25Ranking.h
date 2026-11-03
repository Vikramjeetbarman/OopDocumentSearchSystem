#pragma once

#include "RankingStrategy.h"
#include <string>

class InvertedIndex;
class DocumentStore;

class BM25Ranking : public RankingStrategy {
private:
    const InvertedIndex& index;
    DocumentStore* documentStore;

    static constexpr double k1 = 1.2;
    static constexpr double b  = 0.75;

    double computeIDF(const std::string& term) const;

public:
    explicit BM25Ranking(const InvertedIndex& index,DocumentStore* documentStore);

    std::vector<SearchResult> rank(const Query& query,const std::vector<SearchResult>& results) override;

    const char* getName() const override;
};