#pragma once

class SearchEvent;

class SearchObserver {
public:
    
    virtual ~SearchObserver() = default;

    virtual void onSearchExecuted(const SearchEvent& event) = 0;
};
