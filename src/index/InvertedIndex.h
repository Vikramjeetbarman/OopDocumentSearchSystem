#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../documents/Document.h"

class InvertedIndex {
private:
    // maps a term -> set of document IDs
    // term -> {docId1, docId2, ...}
    std::unordered_map<std::string, std::unordered_map<int,int>> index;

    std::unordered_map<int, int> documentLengths;
    int totalDocuments = 0;
    int totalDocumentLength = 0;

public:
    InvertedIndex() = default;

    void addDocument(const Document& document);
    void removeDocument(int docId);
    std::unordered_set<int> lookup(const std::string& term) const;
    int getTermFrequency(const std::string& term, int docId) const;
    int getDocumentFrequency(const std::string& term) const;
    bool empty() const;
    void clear();
    int getDocumentLength(int docId) const;
    double getAverageDocumentLength() const;
    int getTotalDocuments() const;
    const std::unordered_map<std::string,std::unordered_map<int,int>>& getIndex() const;

};
