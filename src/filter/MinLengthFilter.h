#pragma once

#include "SearchFilter.h"
#include "../util/StringUtil.h"

class MinLengthFilter : public SearchFilter {
private:
    size_t minTokenCount;

public:
    explicit MinLengthFilter(size_t minTokenCount)
        : minTokenCount(minTokenCount) {}

    bool apply(const Document& document) const override {

        auto tokens = StringUtil::tokenize(document.getContent());

        return tokens.size() >= minTokenCount;
    }
};