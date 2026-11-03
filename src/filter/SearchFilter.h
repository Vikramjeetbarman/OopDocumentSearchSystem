#pragma once

#include "../documents/Document.h"

class SearchFilter {
public:
    virtual bool apply(const Document& document) const = 0;
    virtual ~SearchFilter() = default;
};