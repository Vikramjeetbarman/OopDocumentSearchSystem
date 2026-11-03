#include "SearchHistory.h"
#include "../core/Query.h"
#include "../observer/SearchEvent.h"

SearchHistory::SearchHistory() {}

std::vector<SearchEvent>& SearchHistory::getHistory() {
    return history;
}

void SearchHistory::onSearchExecuted(const SearchEvent& event) {
    history.push_back(event);
}