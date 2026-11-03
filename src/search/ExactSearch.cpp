#include "ExactSearch.h"
#include "../core/Query.h"
#include "../documents/Document.h"
#include "../util/StringUtil.h"
#include "../index/InvertedIndex.h"
#include "SearchContext.h"

// ExactSearch::ExactSearch(const std::vector<Document>& documents)
// :documents(documents){}
ExactSearch::ExactSearch(){}



std::vector <SearchResult> ExactSearch::search(const Query& query,const std::vector<Document>& documents,const SearchContext& context){

    std::vector<SearchResult> exactSearchResults;
    const std::string& queryText=query.getQueryText();
    std::vector<std::string>tokens= StringUtil::tokenize(queryText);

    if(tokens.empty()) return exactSearchResults;

    std::unordered_set<int>docs=context.index.lookup(tokens[0]);

    for(size_t i=1;i<tokens.size();i++){
        std::unordered_set<int>docList=context.index.lookup(tokens[i]);

        std::unordered_set<int>candidate;

        for(int doc:docList){
            if(docs.find(doc)!=docs.end()){
                candidate.insert(doc);
            }
        }

        docs=candidate;
        if(candidate.empty()){
            break;
        }
    }

    if(docs.empty())return exactSearchResults;

    for(const Document& document :documents){

        if(docs.find(document.getId())==docs.end()) continue;

        exactSearchResults.emplace_back(document.getId(), queryText);

    }
    return exactSearchResults;
}



