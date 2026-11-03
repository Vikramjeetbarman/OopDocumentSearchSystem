#include "FilterChain.h"

void FilterChain::addFilter(std::unique_ptr<SearchFilter> filter){
    filters.push_back(std::move(filter));
}

std::vector<Document> FilterChain::apply(const std::vector<Document>& documents) const{
    std::vector<Document> result;

    for (const auto& doc : documents) {

        bool passesAll = true;

        for (const auto& filter : filters) {
            if (!filter->apply(doc)) {
                passesAll = false;
                break;
            }
        }

        if (passesAll) {
            result.push_back(doc);
        }
    }

    return result;
}