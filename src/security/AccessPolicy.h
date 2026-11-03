#pragma once

#include <vector>

class Document;
class Query;

class AccessPolicy {
public:
    virtual ~AccessPolicy() = default;

    virtual std::vector<Document> filter(
        const std::vector<Document>& documents,
        const Query& query) const = 0;
};