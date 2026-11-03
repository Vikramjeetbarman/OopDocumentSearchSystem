#pragma once

#include <string>
#include <functional>

#include "../core/SearchScope.h"

class CacheKey {
private:
    std::string queryText;
    std::string rankingName;

    SearchScope scope;
    int userId;

    std::string authorFilter;
    int minLengthFilter;
    std::string dateRangeFilter;

public:
   CacheKey(const std::string& queryText,
        const std::string& rankingName,
         SearchScope scope,
         int userId,
         const std::string& authorFilter,
         int minLengthFilter,
         const std::string& dateRangeFilter)
    : queryText(queryText),
      rankingName(rankingName),
      scope(scope),
      userId(userId),
      authorFilter(authorFilter),
      minLengthFilter(minLengthFilter),
      dateRangeFilter(dateRangeFilter) {}

    const std::string& getQueryText() const {
        return queryText;
    }

    const std::string& getRankingName() const {
        return rankingName;
    }

    SearchScope getScope() const {
    return scope;
    }

    int getUserId() const {
        return userId;
    }

    const std::string& getAuthorFilter() const {
        return authorFilter;
    }

    int getMinLengthFilter() const {
        return minLengthFilter;
    }

    const std::string& getDateRangeFilter() const {
        return dateRangeFilter;
    }

    bool operator==(const CacheKey& other) const {
    return queryText == other.queryText &&
           rankingName == other.rankingName &&
           scope == other.scope &&
           userId == other.userId &&
           authorFilter == other.authorFilter &&
           minLengthFilter == other.minLengthFilter &&
           dateRangeFilter == other.dateRangeFilter;
    }
};

class CacheKeyHash {
public:
    std::size_t operator()(const CacheKey& key) const {
    std::hash<std::string> strHasher;
    std::hash<int> intHasher;
    std::size_t h = 0;

    h ^= strHasher(key.getQueryText());
    h ^= strHasher(key.getRankingName()) << 1;
    h ^= intHasher(static_cast<int>(key.getScope())) << 2;
    h ^= intHasher(key.getUserId()) << 3;
    h ^= strHasher(key.getAuthorFilter()) << 4;
    h ^= intHasher(key.getMinLengthFilter()) << 5;
    h ^= strHasher(key.getDateRangeFilter()) << 6;

    return h;
    }
};
