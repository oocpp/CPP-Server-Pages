#pragma once
#include<thread>
#include"TaskQueue.h"
#include<atomic>

class Web
{
public:
	Web(const Web&) = delete;
	Web& operator=(const Web&) = delete;

	Web() noexcept;
	~Web() noexcept;
	void start()noexcept;
    void stop()noexcept ;

private:
	void prepare();

    void startAccept()noexcept ;
	void startRead();
	void startWrite();
	void startHandle();

    void notifyInit(){
        ++_count;
        _cnd.notify_one();
    }

    void notifyExit(){
        --_count;
        _cnd.notify_one();
    }

    bool _doing;
	std::atomic<int>_count;
    std::thread _th_accept;
	std::thread _th_read;
	std::thread _th_handle;
	std::thread _th_write;

	TaskQueue _task_queue;

    std::mutex _m;
    std::condition_variable _cnd;
};