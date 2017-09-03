#pragma once
#include<thread>
#include"TaskQueue.h"

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
    void startAccept()noexcept ;
	void startRead();
	void startWrite();
	void startHandle();

    bool _doing;
    std::thread _th_accept;
	std::thread _th_read;
	std::thread _th_handle;
	std::thread _th_write;

	TaskQueue _task_queue;
};