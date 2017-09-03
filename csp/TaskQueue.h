//
// Created by lg on 17-8-30.
//

#pragma once

#include<queue>
#include<map>
#include<atomic>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<utility>
#include<algorithm>
#include<chrono>
#include"DebugPrint.h"
#include"Data.h"

class TaskQueue{
public:
    void addReadTask(int so){
        std::lock_guard<std::mutex> l(_read_mutex);
        _read_queue.push(so);
    }

    void addHandleTask(Data&data){
        std::lock_guard<std::mutex> l(_handle_mutex);
        _handle_queue.push(data);
    }

    void addWriteTask(Data&data){
        std::lock_guard<std::mutex> l(_write_mutex);
        _write_queue.push(data);
    }

    std::queue<int> getReadTaskQueue(){
        std::queue<int> q;
        {
            std::lock_guard<std::mutex> l(_read_mutex);
            q.swap(_read_queue);
        }
        return q;
    }

    std::queue<Data> getHadleTaskQueue(){
        std::queue<Data> q;
        {
            std::lock_guard<std::mutex> l(_handle_mutex);
            q.swap(_handle_queue);
        }
        return q;
    }

    std::queue<Data> getWriteTaskQueue(){
        std::queue<Data> q;
        {
            std::lock_guard<std::mutex> l(_write_mutex);
            q.swap(_write_queue);
        }
        return q;
    }

    void waitReadQueue() {
        std::unique_lock<std::mutex> ul(_read_mutex);
        _readCond.wait(ul , [this] { return !_read_queue.empty(); });
    }

    void waitHandleQueue(){
        std::unique_lock<std::mutex> ul(_handle_mutex);
        _handleCond.wait(ul , [this] { return !_handle_queue.empty(); });
    }

    void waitWriteQueue(){
        std::unique_lock<std::mutex> ul(_write_mutex);
        _writeCond.wait(ul, [this] { return !_write_queue.empty(); });
    }

    void notifyRead(){
        _readCond.notify_one();
    }

    void notifyHandle(){
        _handleCond.notify_one();
    }

    void notifyWrite(){
        _writeCond.notify_one();
    }

    void exit(){

        _read_queue.push(-1);
        _handle_queue.push(Data(-1));
        _write_queue.push(Data(-1));;

        notifyRead();
        notifyHandle();
        notifyWrite();
    }

private:
    std::mutex _read_mutex;
    std::mutex _handle_mutex;
    std::mutex _write_mutex;

    std::condition_variable _readCond;
    std::condition_variable _handleCond;
    std::condition_variable _writeCond;

    std::queue<int> _read_queue;
    std::queue<Data> _handle_queue;
    std::queue<Data> _write_queue;
};
