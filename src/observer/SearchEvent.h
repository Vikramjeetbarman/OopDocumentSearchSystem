#pragma once

#include "../core/SearchResult.h"
#include "../core/Query.h"

#include<vector>
#include<chrono>

class SearchEvent {

private:
    
    int userId;
    const std::string queryText;


    std::vector<SearchResult> results;
    
    
    std::chrono::system_clock::time_point timestamp;

public:
   
    SearchEvent(int userId,const std::string& queryText,const std::vector<SearchResult>& results)
    :userId(userId),queryText(queryText),results(results),timestamp(std::chrono::system_clock::now()){}

    int getUserId()const {
        return userId;
    }

    const std::string& getQueryText() const{
        return queryText;
    }

    int getResultCount() const{
        return results.size();
    }

    const std::vector<SearchResult>& getResults() const{
        return results;
    }

    std::chrono::system_clock::time_point getTimestamp() const {
        return timestamp;
    }
   
};
