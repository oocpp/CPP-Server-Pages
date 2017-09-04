#pragma once
#include<string>
#include<vector>
#include<regex>
#include<utility>
#include"Buffer.h"

class HttpCode;


class Request {
	std::vector<std::pair<std::smatch::value_type, std::smatch::value_type>>head;
	std::vector<std::pair<std::smatch::value_type, std::smatch::value_type>>arg;
	thread_local static std::string recvData;
	std::string url;
	std::string fileType;
	std::smatch::value_type method;
public:
	Request(Buffer&data);

	std::string getMethod()const;
	const std::string& getUrl()const;
	std::string getHeader(const std::string&s)const;
	std::string getArg(const std::string&s)const;
	const std::string& getFileType()const;

	bool analysis(HttpCode & httpCode);
	bool filter();
	bool redirect(HttpCode & httpCode);
};
