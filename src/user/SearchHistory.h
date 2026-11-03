#pragma once

#include <vector>
#include "../observer/SearchObserver.h"
#include "../observer/SearchEvent.h"

class Query;

 //Maintains a record of searches performed by a user.
 
class SearchHistory :public SearchObserver {
private:
    // Stores queries in the order they were executed
    std::vector<SearchEvent> history;

public:
    
    SearchHistory();

    std::vector<SearchEvent>& getHistory();

     void onSearchExecuted(const SearchEvent& event) override;
};
