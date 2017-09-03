//
// Created by lg on 17-9-2.
//

#pragma once
#include<vector>

struct Data{
    Data(int so):sockfd(so),buf(1024),pos(0){}
    int sockfd;
    std::vector<char>buf;
    size_t pos;

    char* writeBegin(){
        return buf.data()+pos;
    }

    size_t writableSize() {
        return buf.size() - pos;
    }
};