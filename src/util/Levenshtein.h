#pragma once
#include <string>

class Levenshtein {
public:
    static int distance(const std::string& a, const std::string& b);
};