#pragma once

#include "Visibility.h"
#include <string>
#include<chrono>

class Document{
    
private:
    
    int id;
    int userId;
    std::string title;
    std::string content;
    Visibility visibility;
    std::chrono::system_clock::time_point createdAt;
    int popularityScore = 0;

public:
    
    Document(int id,int userId,const std::string&title,const std::string& content, Visibility visibility)
    :id(id),userId(userId),title(title),content(content),visibility(visibility),createdAt(std::chrono::system_clock::now()){}

     Document(int id,int userId,const std::string&title,const std::string& content, Visibility visibility,std::chrono::system_clock::time_point createdAt)
    :id(id),userId(userId),title(title),content(content),visibility(visibility),createdAt(createdAt){}

    int getId() const{
        return id;
    }

    int getUserId() const{
        return userId;
    }

    const std::string& getContent() const{
        return content;
    }
    
    Visibility getVisibility() const{
        return visibility;
    }

    std::chrono::system_clock::time_point getCreatedAt() const {
        return createdAt;
    }

    void setVisibility(Visibility visibility){
        this->visibility=visibility;
    }

    const std::string& getTitle() const {
    return title;
    }

    void recordImpression() {
        popularityScore++;
    }

    int getPopularityScore() const {
        return popularityScore;
    }

    void setPopularityScore(int score) {
    popularityScore = score;
    }

    void setTitle(const std::string& title) {
    this->title = title;
    }

    void setContent(const std::string& content) {
        this->content = content;
    }

};


