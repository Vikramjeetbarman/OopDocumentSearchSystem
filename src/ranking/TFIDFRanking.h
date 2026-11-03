#pragma once

#include "RankingStrategy.h"
#include <string>
#include <vector>

class InvertedIndex;
class Query;
class SearchResult;

// TF-IDF based ranking strategy
// - Uses binary TF (term present or not)
// - Uses IDF from inverted index
// - Ignores positional information
class TFIDFRanking : public RankingStrategy {
private:
    const InvertedIndex& index;

    // Helper methods
    double computeIDF(const std::string& term) const;
    double computeScore(const std::vector<std::string>& queryTokens,int documentId) const;

public:
    TFIDFRanking(const InvertedIndex& index);

    std::vector<SearchResult>rank(const Query& query,const std::vector<SearchResult>& results) override;

    const char* getName() const override;
};
