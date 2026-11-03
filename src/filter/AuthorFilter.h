#pragma once

#include "SearchFilter.h"

class AuthorFilter : public SearchFilter {
private:
    int authorId;

public:
    explicit AuthorFilter(int authorId)
        : authorId(authorId) {}

    bool apply(const Document& document) const override {
        return document.getUserId() == authorId;
    }
};