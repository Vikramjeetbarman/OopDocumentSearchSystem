#include "BM25Ranking.h"
#include "../index/InvertedIndex.h"
#include "../core/SearchResult.h"
#include "../core/Query.h"
#include "../util/StringUtil.h"
#include "../documents/DocumentStore.h"
#include "../documents/Document.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

BM25Ranking::BM25Ranking(const InvertedIndex& index,DocumentStore* documentStore)
    : index(index),documentStore(documentStore) {}


// --- IDF computation ---
double BM25Ranking::computeIDF(const std::string& term) const {
    int df = index.getDocumentFrequency(term);
    int N  = index.getTotalDocuments();

    if (df == 0 || N == 0)
        return 0.0;

    // Standard BM25 IDF
    return std::log(((N - df + 0.5) / (df + 0.5)) + 1.0);
}
static bool comp(const SearchResult &a,const SearchResult &b){
    return a.getScore()>b.getScore();
}

std::vector<SearchResult> BM25Ranking::rank(const Query& query,const std::vector<SearchResult>& results){
    std::vector<SearchResult> rankedResults = results;

    std::vector<std::string> queryTerms =StringUtil::tokenize(query.getQueryText());

    if (queryTerms.empty())
        return rankedResults;

    double avgDL = index.getAverageDocumentLength();
    if (avgDL == 0.0)
        return rankedResults;

    for ( SearchResult& result : rankedResults) {

        int docId = result.getDocumentId();
        int docLength = index.getDocumentLength(docId);

        auto docOpt = documentStore->getDocumentById(docId);
        if (!docOpt.has_value()) continue;

        const Document* doc = docOpt.value();

    std::vector<std::string> titleTokensInit =StringUtil::tokenize(doc->getTitle());

    std::unordered_set<std::string> titleTokens(titleTokensInit.begin(),titleTokensInit.end());

        double score = 0.0;
        double boost=1.0;
        switch (result.getMatchType()) {
    case MatchType::PHRASE: boost = 2.0; break;
    case MatchType::EXACT:  boost = 1.5; break;
    case MatchType::PREFIX: boost = 1.2; break;
    case MatchType::FUZZY:  boost = 1.0; break;
}
        
     for (const std::string& term : queryTerms) {

            int tf = index.getTermFrequency(term, docId);
            if (tf == 0) continue;

            double idf = computeIDF(term);

            double numerator = tf * (k1 + 1.0);

            double denominator =
                tf + k1 * (1.0 - b + b * (static_cast<double>(docLength) / avgDL));

            score += idf * (numerator / denominator);
            if (titleTokens.count(term)) {
                score += idf * 2.0;
            }
        }
        score*=boost;
        result.setScore(score);
    }

        std::sort(rankedResults.begin(), rankedResults.end(),comp);


    return rankedResults;
}


const char* BM25Ranking::getName() const {
    return "BM25Ranking";
}