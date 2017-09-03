#pragma once
#include<queue>
#include"Data.h"
#include "TaskQueue.h"

class Session {
public:
	Session(TaskQueue&tq):_queue(tq){}
	void handle();
private:
    TaskQueue&_queue;
};