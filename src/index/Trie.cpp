#include "Trie.h"
#include "../util/StringUtil.h"

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie(){
    clear();
}

void Trie::clear(){
    if (!root) return;
    clearNode(root);
    root = new TrieNode();
}

void Trie::clearNode(TrieNode* node){
    if (!node) return;

    for (auto& [ch, child] : node->children) {
        if (child) {
            clearNode(child);
        }
    }

    delete node;
}


Trie::TrieNode::TrieNode() : isTerminal(false) {}

void Trie::insert(const std::string&word){
    TrieNode* current=root;
        
    for(auto c:word){
        if(current->children.find(c)==current->children.end()){
            current->children[c]=new TrieNode;
        }
        current=current->children[c];
    }

    current->isTerminal=true;
}


void Trie::insertDocument(const Document& doc){

    std::string content=doc.getContent();
    std::string title=doc.getTitle();
    std::vector<std::string> tokens = StringUtil::tokenize(title+" "+content);
    for(auto & token:tokens){
        insert(token);
    }
}


void Trie::collectWords(TrieNode* node,std::string& current,std::vector<std::string>& result) const{
    if(node->isTerminal) result.push_back(current);

    for(const auto& [ch, child] : node->children){
        if(child){
            current.push_back(ch);
            collectWords(child,current,result);
            current.pop_back();
        }
    }
}

std::vector<std::string> Trie::getWordsWithPrefix(const std::string& prefix) const{

    TrieNode*current=root;
    for(auto c:prefix){
        if(current->children.find(c)!=current->children.end()){
            current=current->children[c];
        }
        else {
            return {};
        }
    }

    std::vector<std::string> ans;
    std::string temp=prefix;
    collectWords(current,temp,ans);

    return ans;
}

