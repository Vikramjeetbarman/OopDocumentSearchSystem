#pragma once
#include<unordered_map>
#include<vector>
#include "../documents/Document.h"
#include "../documents/DocumentStore.h"

// Concrete implementation of DocumentStore.

class FileStorage :  public DocumentStore{
private:
    std::string filePath;
    std::unordered_map<int,Document>mapDocs;
    int nextDocId = 1;
   
    

public:
    explicit FileStorage(const std::string& filePath);

    void addDocument(const Document& document)override;
    void loadFromFile() override;
    void flushToDisk() override;
    int generateDocId() override;
    std::optional< Document*> getDocumentById(int id)  override;
    std::vector<Document*> getDocumentsByUser(int userId) override;
    void updateDocument(const Document& updatedDocument) override;
    
    void removeDocument(int docId) override;

    std::vector<Document> getAllDocuments()const override;

};

