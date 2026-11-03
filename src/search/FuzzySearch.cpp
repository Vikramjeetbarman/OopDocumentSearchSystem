#include "FuzzySearch.h"
#include "../index/InvertedIndex.h"
#include "../util/StringUtil.h"
#include "../util/Levenshtein.h"
#include "../core/SearchResult.h"
#include "../core/Query.h"


#include <unordered_set>
#include <vector>

FuzzySearch::FuzzySearch(){}
    

std::vector<SearchResult> FuzzySearch::search(const Query& query,const std::vector<Document>&,const SearchContext& context) {

    std::vector<SearchResult> results;
    std::string queryText=query.getQueryText();
    std::vector<std::string> queryTerms =StringUtil::tokenize(queryText);

    if (queryTerms.empty())
        return results;


    const InvertedIndex& index=context.index;
    const auto& fullIndex = index.getIndex();

    std::vector<std::unordered_set<int>> termDocSets;

    for (const std::string& queryTerm : queryTerms) {

        int threshold = 0;

        if (queryTerm.length() <= 3)
            threshold = 0;
        else if (queryTerm.length() <= 6)
            threshold = 1;
        else
            threshold = 2;

        std::unordered_set<int> currentTermDocs;

        for (const auto& [term, postings] : fullIndex) {

            int dist = Levenshtein::distance(queryTerm, term);

            if (dist <= threshold) {

                for (const auto& [docId, freq] : postings) {
                    currentTermDocs.insert(docId);
                }
            }
        }

        termDocSets.push_back(currentTermDocs);
    }

    if (termDocSets.empty())
        return results;

    // --- INTERSECTION (AND semantics) ---

    std::unordered_set<int> finalDocs = termDocSets[0];

    for (size_t i = 1; i < termDocSets.size(); i++) {

        std::unordered_set<int> temp;

        for (int docId : finalDocs) {
            if (termDocSets[i].count(docId)) {
                temp.insert(docId);
            }
        }

        finalDocs = std::move(temp);

        if (finalDocs.empty())
            break;
    }

    for (int docId : finalDocs) {
        results.emplace_back(docId,queryText);
    }

    return results;
}

