#pragma once

#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace StringUtil{

    inline std::string resolveProjectPath(const std::string& relativePath) {
        std::filesystem::path input(relativePath);

        if (input.is_absolute()) {
            return input.string();
        }

        for (std::filesystem::path dir = std::filesystem::current_path(); ; dir = dir.parent_path()) {
            std::filesystem::path candidate = dir / input;

            if (std::filesystem::exists(candidate) && std::filesystem::is_regular_file(candidate)) {
                return candidate.string();
            }

            if (dir == dir.parent_path()) {
                break;
            }
        }

        return std::filesystem::absolute(input).string();
    }

    inline std::unordered_set<std::string>loadStopwords(const std::string& file){

        std::ifstream fin(file);
        std::unordered_set<std::string>stopWords;
        std::string word;

        if(!fin.is_open()){
            throw std::runtime_error("Failed to open stopwords file: " + file);
        }

        while(std::getline(fin,word)){
            if(!word.empty())
            stopWords.insert(word);
        }

        return stopWords;
    }

    inline const std::unordered_set<std::string>& getStopWords(){

        static std::unordered_set<std::string>stopwords = loadStopwords(resolveProjectPath("resources/stopwords.txt"));
        return stopwords;
    }

    inline std:: vector<std ::string> normalize(const std:: vector<std ::string>&words){
        
        const std::unordered_set<std::string>& stopwords=getStopWords();
        std:: vector<std ::string> newWords;
        newWords.reserve(words.size());

        for (const std::string& word : words) {
            std:: string newWord;
            for(char c : word){
                if(std::isalnum(static_cast<unsigned char>(c))){
                    newWord.push_back(std::tolower(static_cast<unsigned char>(c)));
                }
            }
            if(!newWord.empty()&&stopwords.find(newWord)==stopwords.end()){
                newWords.push_back(newWord);
            }
        }
        return newWords;
    }

    inline std:: vector<std ::string> tokenize(const std:: string& text){

        std:: vector<std ::string> tokens;
        std::stringstream ss(text);
        std::string word;
        
        while(ss >> word){
            tokens.push_back(word);
        }
        
        return normalize(tokens);
    }

}