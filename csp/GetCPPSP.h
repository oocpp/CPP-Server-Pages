#pragma once
#include<string>
class Response;
class Request;

class GetCPPSP
{
	bool getFile(const std::string & name, std::string& str);
	bool makeCpp(std::string & scp, const std::string & st, Response&response);
public:
	bool operator()(Request&request, Response&response);
};