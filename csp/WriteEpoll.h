//
// Created by lg on 17-8-30.
//

#pragma  once
#include"Epoll.h"
#include<vector>
#include<list>
#include<algorithm>
#include<regex>
#include <sys/socket.h>
#include"Buffer.h"
#include"TaskQueue.h"


class WriteEpoll{
private:
    union TypeCast{
        explicit TypeCast(void*ptr):ptr(ptr){}
        explicit TypeCast(std::list<Buffer>::iterator iter):iter(iter){}
        void *ptr;
        std::list<Buffer>::iterator iter;
    };

    void deleteSocket(std::list<Buffer>::iterator iter){
        _poll.eventDel(iter->sockfd);
        _data_list.erase(iter);
        dout<<"send finsh";
    }

    bool WriteData(Buffer&data) {
        int num = ::write(data.sockfd, data.writeBegin(), data.writableSize());

        if (0 < num) {
            if(num == data.writableSize()) {
                ::close(data.sockfd);
                return true;
            }
            else{
                data.writedSize(num);
                return false;
            }

        }
        else if (num == 0 || errno != EINTR && errno != EAGAIN /*&&errno !=  EWOULDBLOCK*/) {
            ::close(data.sockfd);
            return true;
        }

        return false;
    }

public:
    WriteEpoll(TaskQueue&tq,int timeout=0):_queue(tq),_timeout(timeout),_poll("Write") {

    }

    void wait(){

        _activeEvents.resize(_data_list.size());
        if(_poll.wait(0, _activeEvents)) {
            for (auto &event:_activeEvents) {
                auto iter = TypeCast(event.data.ptr).iter;
                if (WriteData(*iter))
                    deleteSocket(iter);
            }
        }

        std::queue<Buffer> q = _queue.getWriteTaskQueue();

        if(_data_list.empty()&&q.empty()){
            std::cout<<"wait write"<<std::endl;
            _queue.waitWriteQueue();
            std::cout<<"wait write +++"<<std::endl;
            q= _queue.getWriteTaskQueue();
        }

        while(!q.empty()){
            _data_list.emplace_back(q.front());
            if(!WriteData(_data_list.back())) {
                add(--_data_list.end());
            }
            else{
                _data_list.pop_back();
            }
            q.pop();
        }
    }

    void add(std::list<Buffer>::iterator iter){
        _poll.eventAdd(iter->sockfd,Epoll::WRITE, TypeCast(iter).ptr);
    }
private:
    int _timeout;
    Epoll _poll;
    std::vector<epoll_event> _activeEvents;
    std::list<Buffer>_data_list;
    TaskQueue& _queue;
};