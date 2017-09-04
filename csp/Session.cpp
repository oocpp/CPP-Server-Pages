#include <unistd.h>
#include <iostream>
#include"Session.h"
#include"Request.h"
#include"Response.h"
#include"HttpCode.h"
#include"Buffer.h"
#include"TaskQueue.h"


void Session::handle()
{
	enum class Status { RECVIVE, ANALYSIS, FILTER, REDIRECT, GETFILE, SEND,CLOSE,EXIT};

	std::queue<Buffer> queue = _queue.getHadleTaskQueue();
	if(queue.empty()){
        std::cout<<"wait handle"<<std::endl;
		_queue.waitHandleQueue();
		queue = _queue.getHadleTaskQueue();
        std::cout<<"wait handle+++++"<<std::endl;
	}

	while(!queue.empty()) {
		Status s = Status::ANALYSIS;

		Request request(queue.front());
		Response response(_queue);
		HttpCode httpCode;

		while (s != Status::EXIT)
			switch (s) {
				case Status::ANALYSIS:
					s = request.analysis(httpCode) ? Status::FILTER : Status::REDIRECT;
					break;
				case Status::FILTER:
					s = request.filter(/*request*/) ? Status::GETFILE : Status::CLOSE;
					break;
				case Status::GETFILE:
					s = response.getFile(request, httpCode) ? Status::SEND : Status::REDIRECT;
					break;
				case Status::REDIRECT:
					s = request.redirect(httpCode) ? Status::GETFILE : Status::CLOSE;
					break;
				case Status::SEND:
					response.send(queue.front(),httpCode);
					s = Status::EXIT;
					break;
                case Status::CLOSE:
                    ::close(queue.front().sockfd);
                    s = Status::EXIT;
                    break;
			}

		queue.pop();
	}
    _queue.notifyWrite();
}
