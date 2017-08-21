#pragma once
#include<string>

class HttpCode
{
	int code;
public:
	HttpCode();
	~HttpCode();
	void setCode(int c);
	int getCode()const;
	std::string getCodeStr()const;
};

