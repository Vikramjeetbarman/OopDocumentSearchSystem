#pragma once

#include "AccessPolicy.h"

class DefaultAccessPolicy : public AccessPolicy {
public:
    std::vector<Document> filter(
        const std::vector<Document>& documents,
        const Query& query) const override;
};