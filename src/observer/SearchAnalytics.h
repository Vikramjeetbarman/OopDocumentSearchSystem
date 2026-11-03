#pragma once

#include "SearchObserver.h"

#include <unordered_map>
#include <string>

class SearchAnalytics : public SearchObserver
{
private:

    int totalSearches = 0;

    std::unordered_map<std::string,int>
        queryFrequency;

public:

    void onSearchExecuted(
        const SearchEvent& event) override;

    int getTotalSearches() const;

    const std::unordered_map<
        std::string,
        int>& getQueryFrequency() const;
};