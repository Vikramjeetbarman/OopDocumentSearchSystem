#pragma once

class InvertedIndex;
class Trie;
class SearchContext{
public:
    InvertedIndex& index;
    Trie* trie;


    SearchContext(InvertedIndex& index, Trie* trie)
        : index(index), trie(trie) {}
};