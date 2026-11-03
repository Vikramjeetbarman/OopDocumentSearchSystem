#include "PrefixSearch.h"
#include "../core/Query.h"
#include "../documents/Document.h"
#include "../util/StringUtil.h"
#include "../index/InvertedIndex.h"
#include "../index/Trie.h"

// ExactSearch::ExactSearch(const std::vector<Document>& documents)
// :documents(documents){}
PrefixSearch::PrefixSearch(){}
   



std::vector<SearchResult> PrefixSearch::search(const Query& query,const std::vector<Document>& documents,const SearchContext& context){

    std::vector<SearchResult> prefixSearchResults;
    const std::string& queryText=query.getQueryText();
    std::vector<std::string>prefixTokens= StringUtil::tokenize(queryText);

    if(!context.trie) return prefixSearchResults;
    if(prefixTokens.empty()) return prefixSearchResults;

    std::unordered_set<int>docs;
    bool firstToken=true;
    
    for(size_t i=0;i<prefixTokens.size();i++){
        std::vector<std::string>tokens=context.trie->getWordsWithPrefix(prefixTokens[i]);

        std::unordered_set<int>preDocs;

        for(size_t j=0;j<tokens.size();j++){
            std::unordered_set<int>docList=context.index.lookup(tokens[j]);

            for(int doc:docList){
                    preDocs.insert(doc);
            }
        }
        
        if(firstToken){
            docs=preDocs;
            firstToken=false;
        }
        else{
        std::unordered_set<int>candidate;
        for(auto doc:preDocs){
            if(docs.find(doc)!=docs.end()){
                candidate.insert(doc);
            }
        }
        docs=candidate;

        if(docs.empty()) break;
        }
    }

    if(docs.empty())return prefixSearchResults;

    for(const Document& document :documents){

        if(docs.find(document.getId())==docs.end()) continue;
        prefixSearchResults.emplace_back(document.getId(), queryText);
    }
    return prefixSearchResults;
}



