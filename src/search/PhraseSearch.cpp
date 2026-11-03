#include "PhraseSearch.h"
#include "../core/Query.h"
#include "../documents/Document.h"
#include "../util/StringUtil.h"
#include "../index/InvertedIndex.h"
#include "SearchContext.h"

// ExactSearch::ExactSearch(const std::vector<Document>& documents)
// :documents(documents){}
PhraseSearch::PhraseSearch(){}


std::string PhraseSearch::normalizeText(const std:: string&text){
     std:: vector<std ::string> words;
        std::stringstream ss(text);
        std::string word;
        
        while(ss >> word){
            words.push_back(word);
        }

        std:: vector<std ::string> newWords;
        newWords.reserve(words.size());

        for (const std::string& word : words) {
            std:: string newWord;
            for(char c : word){
                if(std::isalnum(static_cast<unsigned char>(c))){
                    newWord.push_back(std::tolower(static_cast<unsigned char>(c)));
                }
            }
            if(!newWord.empty()){
                newWords.push_back(newWord);
            }
        }

        std::string newText;

        for(std::string& word:newWords){
            newText=newText+word+" ";
        }

        if(!newText.empty())
        newText.pop_back();

        return newText;
}

std::vector <SearchResult> PhraseSearch::search(const Query& query,const std::vector<Document>& documents,const SearchContext& context){

    std::vector<SearchResult> phraseSearchResults;
    const std::string& queryText=query.getQueryText();
     std::string newQueryText=normalizeText(queryText);

    std::vector<std::string>tokens= StringUtil::tokenize(queryText);

    if(tokens.empty()) return phraseSearchResults;

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

    if(docs.empty())return phraseSearchResults;

    for(const Document& document :documents){

        if(docs.find(document.getId())==docs.end()) continue;

        const std::string& text=document.getTitle() +" "+ document.getContent();

        std::string newText=normalizeText(text);

        std::size_t pos = newText.find(newQueryText);

        if (pos!= std::string::npos) {
            phraseSearchResults.emplace_back(document.getId(),queryText);
        }
    }
    return phraseSearchResults;
}



