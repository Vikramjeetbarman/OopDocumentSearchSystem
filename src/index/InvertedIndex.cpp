#include "InvertedIndex.h"
#include "../util/StringUtil.h"

void InvertedIndex::addDocument(const Document& document) { 
    int docId=document.getId();
   
    std::string content=document.getContent();
    std::string title=document.getTitle();
    std::vector<std::string> tokens = StringUtil::tokenize(title+" "+content);

    int docLength = tokens.size();
    documentLengths[docId] = docLength;

    totalDocuments++;
    totalDocumentLength += docLength;

    for (const std::string& token : tokens) {
        index[token][docId]++;
    }
}

void InvertedIndex::removeDocument(int docId) {

    auto lenIt = documentLengths.find(docId);
    if (lenIt != documentLengths.end()) {
        totalDocumentLength -= lenIt->second;
        documentLengths.erase(lenIt);
        totalDocuments--;
    }

    for (auto it = index.begin(); it != index.end(); ) {
        it->second.erase(docId);

        if (it->second.empty()) {
            it = index.erase(it);
        } else {
            ++it;
        }
    }
}

 std::unordered_set<int> InvertedIndex::lookup(const std::string& term) const {
    std::unordered_set<int> result;

    auto it = index.find(term);
    if (it == index.end()) {
        return result;
    }

    for (const auto& [docId, freq] : it->second) {
        result.insert(docId);
    }

    return result;
}

int InvertedIndex::getTermFrequency(const std::string& term, int docId) const {
    auto it = index.find(term);
    if (it == index.end()) {
        return 0;
    }

    auto docIt = it->second.find(docId);
    if (docIt == it->second.end()) {
        return 0;
    }

    return docIt->second;
}

int InvertedIndex::getDocumentFrequency(const std::string& term) const {
    auto it = index.find(term);
    if (it == index.end()) {
        return 0;
    }

    return static_cast<int>(it->second.size());
}

bool InvertedIndex::empty() const {
    return index.empty();
}

void InvertedIndex::clear() {
    index.clear();
    documentLengths.clear();
    totalDocuments = 0;
    totalDocumentLength = 0;
}

int InvertedIndex::getDocumentLength(int docId) const {
    auto it = documentLengths.find(docId);
    if (it == documentLengths.end())
        return 0;
    return it->second;
}

double InvertedIndex::getAverageDocumentLength() const {
    if (totalDocuments == 0)
        return 0.0;
    return static_cast<double>(totalDocumentLength) / totalDocuments;
}

int InvertedIndex::getTotalDocuments() const {
    return totalDocuments;
}

const std::unordered_map<std::string,std::unordered_map<int,int>>&InvertedIndex::getIndex() const {
    return index;
}