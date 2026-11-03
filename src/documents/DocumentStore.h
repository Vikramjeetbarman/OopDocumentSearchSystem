#pragma once
#include<vector>
#include<optional>
#include "Document.h"

class DocumentStore{
public:

    virtual void addDocument(const Document& document) = 0;
    virtual std::vector<Document> getAllDocuments() const = 0;
    virtual std::optional< Document*> getDocumentById(int id) = 0;
    virtual std::vector<Document*> getDocumentsByUser(int userId) = 0;
    virtual void updateDocument(const Document& updatedDocument) = 0;
    virtual void removeDocument(int docId) = 0;
    virtual void loadFromFile()=0;
    virtual void flushToDisk()=0;
    virtual int generateDocId()=0;
    virtual ~DocumentStore() = default;

};