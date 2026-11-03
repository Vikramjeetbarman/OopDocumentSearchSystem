#pragma once
#include <stdexcept>
#include <unordered_map>
#include <list>

template <typename Key, typename Value, typename Hash>
class LRUCache {
private:
    size_t capacity;

    std::list<Key> usageList;

   std::unordered_map<Key,std::pair<Value, typename std::list<Key>::iterator>,Hash> cacheMap;

public:
    explicit LRUCache(size_t capacity)
        : capacity(capacity) {}

    bool contains(const Key& key) const {
        return cacheMap.find(key) != cacheMap.end();
    }

    Value get(const Key& key) {

        auto it = cacheMap.find(key);

        if (it == cacheMap.end())
            throw std::runtime_error("Key not found in cache");

        usageList.erase(it->second.second);
        usageList.push_front(key);
        it->second.second = usageList.begin();

        return it->second.first;
    }

    void put(const Key& key, const Value& value) {

        auto it = cacheMap.find(key);

        if (it != cacheMap.end()) {
            usageList.erase(it->second.second);
            usageList.push_front(key);

            it->second.first = value;
            it->second.second = usageList.begin();
            return;
        }

        if (cacheMap.size() >= capacity) {
            Key lruKey = usageList.back();
            usageList.pop_back();
            cacheMap.erase(lruKey);
        }

        usageList.push_front(key);
        cacheMap[key] = { value, usageList.begin() };
    }

    void clear() {
        usageList.clear();
        cacheMap.clear();
    }
};