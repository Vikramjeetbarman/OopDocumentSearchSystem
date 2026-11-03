#pragma once

#include <string>

enum class MatchType {
    PHRASE,
    EXACT,
    PREFIX,
    FUZZY
};

class SearchResult{
private:

    int documentId;
    std:: string matchedText;
    double score;
    MatchType matchType;

public:
    SearchResult(int documentId,const std::string& matchedText)
    :documentId(documentId),matchedText(matchedText),score(0.0),matchType(MatchType::EXACT){}

    int getDocumentId() const{
        return documentId;
    }

    const std:: string& getMatchedText() const{
        return matchedText;
    }


    double getScore() const{
        return score;
    }
    
    void setScore(double score){
        this->score=score;
    }

    void setMatchType(MatchType type) {
        matchType = type;
    }

    MatchType getMatchType() const {
        return matchType;
    }
};