#pragma once
#include<map>
#include<string>
class Request;
class HttpCode;

class Response
{
	std::map<std::string, std::string>response;
	std::string sendData;
public:
	Response();
	~Response();

	bool getFile(Request&request, HttpCode&httpCode);
	bool send(int so, HttpCode&httpCode);
	void setSendData(std::string&&s);
	void setHead(std::string&&name, std::string &&value);
};

