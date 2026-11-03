#include "TFIDFRanking.h"
#include "../core/Query.h"
#include "../core/SearchResult.h"
#include "../util/StringUtil.h"
#include "../index/InvertedIndex.h"

#include<algorithm>
#include<math.h>

/*
 TFIDFRanking
 -------------
 - Ranks search results using TF-IDF
 - Ignores positional information
 - Uses inverted index for statistics
*/

 TFIDFRanking::TFIDFRanking(const InvertedIndex& index)
 :index(index){}
    // ---- Helper methods ----

    // Computes IDF for a single term
    double TFIDFRanking::computeIDF(const std::string& term) const{
        
        int df= index.getDocumentFrequency(term);
        return std::log((static_cast<double>(index.getTotalDocuments())+1.0) / (1.0 + df))+1.0;
    }

    // Computes TF-IDF score for one document
    double TFIDFRanking::computeScore(const std::vector<std::string>& queryTokens,int documentId) const{
        double score=0.0;
        for(const std::string&token:queryTokens){
            score+=computeIDF(token)*index.getTermFrequency(token,documentId);
        }

        return score;
    }

   static bool comp(const SearchResult &a,const SearchResult &b){
    return a.getScore()>b.getScore();
}

    std::vector<SearchResult>TFIDFRanking::rank(const Query& query,const std::vector<SearchResult>& results){

        std::vector<SearchResult> rankedResults = results;

    std::vector<std::string> queryTokens =
        StringUtil::tokenize(query.getQueryText());

    if (queryTokens.empty()) {
        return rankedResults;
    }

    for (SearchResult& result : rankedResults) {
        double score = computeScore(queryTokens, result.getDocumentId());
        result.setScore(score);
    }

    std::sort(rankedResults.begin(), rankedResults.end(),comp);
            

    return rankedResults;

    }

    const char* TFIDFRanking::getName() const {
        return "TFIDFRanking";
    }

