// #pragma once
// #include "SearchObserver.h"
// #include "SearchEvent.h"
// #include "DocumentStore.h"

// class PopularityTracker : public SearchObserver {
// private:
//     DocumentStore* documentStore;

// public:

//     explicit PopularityTracker(DocumentStore* store)
//     :documentStore(store){}

//     void onSearchExecuted(const SearchEvent& event) override{
//         const auto& results = event.getResults();

//     for (const auto& result : results) {
//         auto doc = documentStore->getDocumentById(result.getDocumentId());

//         if (doc.has_value()) {
//             doc.value()->recordImpression();
//         }
    
//     }
//     }
// };