//
// Created by lg on 17-8-30.
//

#pragma once

#include"Epoll.h"
#include<vector>
#include<list>
#include<algorithm>
#include<regex>
#include <sys/socket.h>
#include"Buffer.h"
#include"TaskQueue.h"


class ReadEpoll{
private:
    union TypeCast{
        explicit TypeCast(void*ptr):ptr(ptr){
            assert(sizeof(void*)>=sizeof(std::list<Buffer>::iterator));
        }
        explicit TypeCast(std::list<Buffer>::iterator iter):iter(iter){
            assert(sizeof(void*)>=sizeof(std::list<Buffer>::iterator));
        }
        void *ptr;
        std::list<Buffer>::iterator iter;
    };

    bool isReadComplete(std::vector<char>&buf){
        std::regex re_end("(?:Content-Length:([^\r]+))?[\\s\\S]*\r\n\r\n");
        std::cmatch m;
        if(std::regex_search(static_cast<const char*>(buf.data()),
                             static_cast<const char*>(buf.data()) +buf.size(),
                             m,re_end)) {
            if(m[1].matched) {
                int len = 0;
                for (auto p = m[1].first; p != m[1].second; ++p)
                    len = len * 10 + *p - '0';

                return m.suffix().length() >= len;
            }
            return true;
        }
        return false;
    }

    void deleteSocket(std::list<Buffer>::iterator iter){
        _poll.eventDel(iter->sockfd);
        _data_list.erase(iter);
        dout<<"close read";
    }

    bool readData(Buffer&data) {
        int num = ::recv(data.sockfd, data.writeBegin(), data.writableSize(), 0);

        if (0 < num) {
            if (num == data.writableSize()) {
                data.writedSize(num);
                char buf[65535];
                int num1 = ::recv(data.sockfd, buf, sizeof(buf), 0);

                if (num1 > 0) {
                    data.append(buf,num1);
                }
                else if (num == 0 || errno != EINTR && errno != EAGAIN /*&&errno !=  EWOULDBLOCK*/) {
                    ::close(data.sockfd);
                    return true;
                }
            }
            else{
                data.writedSize(num);
            }

            if (isReadComplete(data.buf)) {
                _queue.addHandleTask(data);
                return true;
            }
            else{
                data.reserve();
            }
        }
        else if (num == 0 || errno != EINTR && errno != EAGAIN /*&&errno !=  EWOULDBLOCK*/) {
            ::close(data.sockfd);
            return true;
        }

        return false;
    }

public:
    ReadEpoll(TaskQueue&tq,int timeout):_queue(tq), _timeout(timeout),_poll("Read") {
    }

    void wait(){
        _activeEvents.resize(_data_list.size());
        if(_poll.wait(0, _activeEvents)) {
            for (auto &event:_activeEvents) {
                auto iter = TypeCast(event.data.ptr).iter;
                if (readData(*iter))
                    deleteSocket(iter);
            }
            _queue.notifyHandle();
        }

        dout<<_data_list.size();
        std::queue<int> q = _queue.getReadTaskQueue();

        if(_data_list.empty()&&q.empty()){
            std::cout<<"wait read"<<std::endl;
            _queue.waitReadQueue();
            std::cout<<"wait read +++++"<<std::endl;
            q = _queue.getReadTaskQueue();
        }

        while (!q.empty()) {
            _data_list.emplace_back(q.front());
            if (!readData(_data_list.back())) {
                add(--_data_list.end());
            }
            else {
                _data_list.pop_back();
            }
            q.pop();
            _queue.notifyHandle();
        }
    }

    void add(std::list<Buffer>::iterator iter){
        _poll.eventAdd(iter->sockfd,Epoll::READ, TypeCast(iter).ptr);
    }
private:
    int _timeout;
    Epoll _poll;
    std::vector<epoll_event> _activeEvents;
    std::list<Buffer>_data_list;
    TaskQueue& _queue;
};