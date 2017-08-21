#include "HttpCode.h"



HttpCode::HttpCode() :code(0)
{
}


HttpCode::~HttpCode()
{
}

void HttpCode::setCode(int c)
{
	code = c;
}

int HttpCode::getCode() const
{
	return code;
}

std::string HttpCode::getCodeStr() const
{
	return std::to_string(code)+" OK";
}
