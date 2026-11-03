#pragma once

#include <vector>
#include <memory>
#include "SearchFilter.h"

class FilterChain {
private:
    std::vector<std::unique_ptr<SearchFilter>> filters;

public:
    FilterChain() = default;

    void addFilter(std::unique_ptr<SearchFilter> filter);

    std::vector<Document> apply(const std::vector<Document>& documents) const;
};