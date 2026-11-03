#pragma once

#include "SearchFilter.h"
#include <chrono>

class DateFilter : public SearchFilter {
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;

public:
    DateFilter(std::chrono::system_clock::time_point start,std::chrono::system_clock::time_point end)
        : start(start), end(end) {}

    bool apply(const Document& document) const override {

        auto createdAt = document.getCreatedAt();

        return createdAt >= start && createdAt <= end;
    }
};