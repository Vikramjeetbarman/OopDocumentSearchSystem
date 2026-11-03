#include "SearchAnalytics.h"
#include "SearchEvent.h"

void SearchAnalytics::onSearchExecuted(
    const SearchEvent& event)
{
    totalSearches++;

    queryFrequency[
        event.getQueryText()
    ]++;
}

int SearchAnalytics::getTotalSearches() const
{
    return totalSearches;
}

const std::unordered_map<
    std::string,
    int>&
SearchAnalytics::getQueryFrequency() const
{
    return queryFrequency;
}