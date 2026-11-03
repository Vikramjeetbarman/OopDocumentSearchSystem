#include "DefaultAccessPolicy.h"
#include "../documents/Document.h"
#include "../core/Query.h"

std::vector<Document> DefaultAccessPolicy::filter(
    const std::vector<Document>& documents,
    const Query& query) const
{
    std::vector<Document> result;

    SearchScope scope = query.getSearchScope();
    int userId = query.getUserId();

    for (const auto& document : documents) {

        bool visible = false;

        if (scope == SearchScope::GLOBAL) {
            visible = (document.getVisibility() == Visibility::PUBLIC);
        }
        else if (scope == SearchScope::PRIVATE) {
            visible = (document.getUserId() == userId);
        }
        else if (scope == SearchScope::BOTH) {
            visible =
                (document.getVisibility() == Visibility::PUBLIC) ||
                (document.getUserId() == userId);
        }

        if (visible) {
            result.push_back(document);
        }
    }

    return result;
}