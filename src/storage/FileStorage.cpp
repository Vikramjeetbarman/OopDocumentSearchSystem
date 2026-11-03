#include "FileStorage.h"

#include <fstream>
#include <sstream>

FileStorage::FileStorage(const std::string& filePath)
    : filePath(filePath) {
        loadFromFile();
    }

void FileStorage::loadFromFile(){
    mapDocs.clear();
    std::ifstream fin(filePath);

    if (!fin.is_open()) {
        return;
    }

    std::string line;

    while (std::getline(fin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, userStr,titleStr, visStr,timeStr, content,popStr;

        std::getline(ss, idStr, '|');
        std::getline(ss, userStr, '|');
        
        std::getline(ss, visStr, '|');
        std::getline(ss, timeStr, '|');
        std::getline(ss, popStr, '|');
        std::getline(ss, titleStr, '|');
        std::getline(ss, content); 

        size_t pos = 0;

        while((pos = content.find("\\n", pos))
            != std::string::npos)
        {
            content.replace(pos, 2, "\n");
            pos += 1;
        }

        int id = std::stoi(idStr);
        int userId = std::stoi(userStr);
        Visibility visibility;

        if (visStr == "PUBLIC")
            visibility = Visibility::PUBLIC;
        else
            visibility = Visibility::PRIVATE;


        std::time_t rawTime = std::stoll(timeStr);
        auto createdAt = std::chrono::system_clock::from_time_t(rawTime);
        Document document(
            id,
            userId,
            titleStr,
            content,
            visibility,
            createdAt
        );
        int popularity =std::stoi(popStr);
        document.setPopularityScore(popularity);
        
        mapDocs.insert_or_assign(id, document);
    }


    nextDocId = 1;

    for (const auto& pair : mapDocs) {
        nextDocId = std::max(nextDocId, pair.first + 1);
    }
    
}

void FileStorage::flushToDisk() {

    std::ofstream fout(filePath, std::ios::trunc);

    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open document file for flush");
    }

    for (const auto& pair : mapDocs) {

        const Document& doc = pair.second;

        std::string visibilityStr =
            (doc.getVisibility() == Visibility::PUBLIC) ? "PUBLIC" : "PRIVATE";

        std::time_t time =
            std::chrono::system_clock::to_time_t(doc.getCreatedAt());

        std::string content = doc.getContent();

        size_t pos = 0;

        while((pos = content.find('\n', pos))
            != std::string::npos)
        {
            content.replace(pos, 1, "\\n");
            pos += 2;
        }

        fout << doc.getId() << "|"
             << doc.getUserId() << "|"
             << visibilityStr << "|"
             << time << "|"
             << doc.getPopularityScore() << "|"
             << doc.getTitle() << "|"
             << content
             << "\n";
    }
}

void FileStorage::addDocument(const Document& document) {
    std::ofstream fout(filePath, std::ios::app);

    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open document file for writing");
    }

    std::string visibilityStr =(document.getVisibility() == Visibility::PUBLIC) ? "PUBLIC": "PRIVATE";

    std::time_t time =std::chrono::system_clock::to_time_t(document.getCreatedAt());


    std::string content = document.getContent();

        size_t pos = 0;

        while((pos = content.find('\n', pos))
            != std::string::npos)
        {
            content.replace(pos, 1, "\\n");
            pos += 2;
        }

    fout << document.getId() << "|"
        << document.getUserId() << "|"
        << visibilityStr << "|"
        << time << "|"
        <<document.getPopularityScore()<<"|"
        <<document.getTitle()<<"|"
        << content
        << "\n";


    mapDocs.insert_or_assign(document.getId(),document);

}


std::vector<Document> FileStorage::getAllDocuments() const {

    
    std::vector<Document> result;
    for (const auto& pair : mapDocs) {
        result.push_back(pair.second);
    }
    return result;

}

std::optional<Document*> FileStorage::getDocumentById(int id) {
    auto it = mapDocs.find(id);
    if (it != mapDocs.end())
        return &(it->second);


    return std::nullopt;

    
}

void FileStorage::updateDocument(const Document& updatedDocument) {

    auto it = mapDocs.find(updatedDocument.getId());
    if (it == mapDocs.end())
        throw std::runtime_error("Document not found for update");

    it->second=updatedDocument;

    
    // // 3️⃣ Rewrite entire file
    // std::ofstream fout(filePath, std::ios::trunc);

    // if (!fout.is_open()) {
    //     throw std::runtime_error("Failed to open document file for update");
    // }
    
    // for (const auto& it : mapDocs) {
    //     const Document& doc=it.second;

    //     std::string visibilityStr = (doc.getVisibility() == Visibility::PUBLIC) ? "PUBLIC" : "PRIVATE";

    //     std::time_t time =std::chrono::system_clock::to_time_t(doc.getCreatedAt());

    //     fout << doc.getId() << "|"
    //          << doc.getUserId() << "|"
    //          << visibilityStr << "|"
    //          << time << "|"
    //          <<doc.getPopularityScore()<<"|"
    //          <<doc.getTitle()<<"|"
    //          << doc.getContent()
    //          << "\n";
    // }
}

void FileStorage::removeDocument(int docId) {

    auto it = mapDocs.find(docId);
    if (it == mapDocs.end())
        throw std::runtime_error("Document not found");

    mapDocs.erase(it);

    // // Rewrite full file
    // std::ofstream fout(filePath, std::ios::trunc);

    // if (!fout.is_open())
    //     throw std::runtime_error("Failed to open document file");

    // for (const auto& pair : mapDocs) {

    //     const Document& doc = pair.second;

    //     std::string visibilityStr =
    //         (doc.getVisibility() == Visibility::PUBLIC) ? "PUBLIC" : "PRIVATE";

    //     std::time_t time =
    //         std::chrono::system_clock::to_time_t(doc.getCreatedAt());

    //     fout << doc.getId() << "|"
    //          << doc.getUserId() << "|"
    //          << visibilityStr << "|"
    //          << time << "|"
    //          <<doc.getPopularityScore()<<"|"
    //          << doc.getTitle() << "|"
    //          << doc.getContent()
    //          << "\n";
    // }
}

int FileStorage::generateDocId() {
    return nextDocId++;
}


std::vector<Document*> FileStorage::getDocumentsByUser(int userId) {

    std::vector<Document*> result;

    for (auto& [id, doc] : mapDocs) {

        if (doc.getUserId() == userId) {
            result.push_back(&doc);
        }
    }

    return result;
}
