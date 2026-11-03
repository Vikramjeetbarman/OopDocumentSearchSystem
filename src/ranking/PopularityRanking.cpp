#include "PopularityRanking.h"
#include "../documents/DocumentStore.h"
#include "core/SearchResult.h"
#include <algorithm>
#include <ctime>
#include<chrono>
#include <cmath>


PopularityRanking::PopularityRanking(DocumentStore* store)
    : documentStore(store) {}

    std::vector<SearchResult> PopularityRanking::rank(const Query&,const std::vector<SearchResult>& results){
        
        std::vector<SearchResult> rankedResults = results;
        std::sort(rankedResults.begin(), rankedResults.end(),
        [this](const SearchResult& a, const SearchResult& b) {
            
            int popA = 0;
            int popB = 0;
            auto now = std::chrono::system_clock::now();
            auto docA = documentStore->getDocumentById(a.getDocumentId());
            if (docA.has_value()){
                auto d = docA.value();

            double ageHours = std::chrono::duration<double>(
                now - d->getCreatedAt()
            ).count() / 3600.0;

    

                popA = d->getPopularityScore() * std::exp(-0.05 * ageHours);
            }

            auto docB = documentStore->getDocumentById(b.getDocumentId());
            if (docB.has_value()){
                auto d = docA.value();

            double ageHours = std::chrono::duration<double>(
                now - d->getCreatedAt()
            ).count() / 3600.0;

          
            popB = d->getPopularityScore() * std::exp(-0.05 * ageHours);
            }
              

            return popA > popB; 
        });

        return rankedResults;
}

const char* PopularityRanking::getName() const{
    return "PopularityRanking";
}
