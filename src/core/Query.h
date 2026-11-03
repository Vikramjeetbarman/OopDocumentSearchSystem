#pragma once

#include <string>
#include "SearchScope.h"
#include<optional>
#include<chrono>

class Query{

private:

    const std:: string queryText;
    const SearchScope searchScope;
    const int userId;

    std::optional<int> authorFilter;
    std::optional<size_t> minLengthFilter;
    std::optional<std::pair<std::chrono::system_clock::time_point,std::chrono::system_clock::time_point>> dateRangeFilter;

public:

    Query(const std::string& queryText,SearchScope searchScope,int userId)
    :queryText(queryText),searchScope(searchScope),userId(userId){}

    int getUserId() const{
        return userId;
    }
    const std::string& getQueryText() const{
        return queryText;
    }
    SearchScope getSearchScope() const{
        return searchScope;
    }

    void setAuthorFilter(int authorId){
        authorFilter=authorId;
    }
    void setMinLengthFilter(size_t minLength){
        minLengthFilter=minLength;
    }
    void setDateRangeFilter(std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point end){
        dateRangeFilter = std::make_pair(start, end);
    }
    const std::optional<int>& getAuthorFilter() const{
        return authorFilter;
    }
    const std::optional<size_t>& getMinLengthFilter() const{
        return minLengthFilter;
    }
    const std::optional<std::pair<std::chrono::system_clock::time_point,std::chrono::system_clock::time_point>>& getDateRangeFilter() const{
        return dateRangeFilter;
    }
    
    void clearAuthorFilter() { authorFilter.reset(); }
    void clearMinLengthFilter() { minLengthFilter.reset(); }
    void clearDateRangeFilter() { dateRangeFilter.reset(); }

};
