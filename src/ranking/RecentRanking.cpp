#include "RecentRanking.h"

#include "../documents/DocumentStore.h"
#include "../documents/Document.h"
#include "../core/SearchResult.h"

#include <algorithm>

RecentRanking::RecentRanking(
    DocumentStore* documentStore)
    : documentStore(documentStore)
{
}

std::vector<SearchResult>
RecentRanking::rank(
    const Query&,
    const std::vector<SearchResult>& results)
{
    std::vector<SearchResult> ranked =
        results;

    std::sort(
        ranked.begin(),
        ranked.end(),
        [&](const SearchResult& a,
            const SearchResult& b)
        {
            auto docA =
                documentStore
                    ->getDocumentById(
                        a.getDocumentId());

            auto docB =
                documentStore
                    ->getDocumentById(
                        b.getDocumentId());

            if(!docA.has_value())
                return false;

            if(!docB.has_value())
                return true;

            return
                docA.value()
                    ->getCreatedAt()
                >
                docB.value()
                    ->getCreatedAt();
        });

    return ranked;
}

const char*
RecentRanking::getName() const
{
    return "Recent Ranking";
}