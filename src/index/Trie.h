#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "documents/Document.h"

class Trie {
private:
    struct TrieNode {
        bool isTerminal;
        std::unordered_map<char, TrieNode*> children;

        TrieNode();
    };

    TrieNode* root;

    void collectWords(TrieNode* node,std::string& current,std::vector<std::string>& result) const;

    void clearNode(TrieNode* node);

public:
    Trie();
    ~Trie();

    void clear();
    void insert(const std::string& word);

    void insertDocument(const Document& doc);

    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) const;

};
