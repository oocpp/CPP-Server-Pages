#pragma once
#include<map>
#include<string>
#include "TaskQueue.h"

class Request;
class HttpCode;

class Response
{
	std::map<std::string, std::string>response;
	std::string sendData;
	TaskQueue &_queue;
public:
	explicit Response(TaskQueue&tq);
	~Response();

	bool getFile(Request&request, HttpCode&httpCode);
	bool send(Buffer&data,HttpCode&httpCode);
	void setSendData(std::string&&s);
	void setHead(std::string&&name, std::string &&value);
};

