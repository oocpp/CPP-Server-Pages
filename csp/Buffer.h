//
// Created by lg on 17-9-2.
//

#pragma once
#include<vector>
#include<cstring>

class Buffer{
public:
    Buffer(int so):sockfd(so),buf(1024),pos(0){
    }

    char*writeBegin(){
        return buf.data()+ pos;
    }

    size_t writableSize(){
        return buf.size() - pos;
    }

    void append(const char*data,size_t s){
        buf.resize(pos + s);
        memmove(writeBegin(), data, s);
        pos+=s;
    }

    void writedSize(size_t s){
        pos+=s;
    }

    void reserve(){
        buf.reserve(buf.size()*2);
    }

    void setSendData(const std::string &str){
        pos=0;
        buf.assign(str.begin(),str.end());
    }

//private:
    int sockfd;
    std::vector<char>buf;
    int pos;
};