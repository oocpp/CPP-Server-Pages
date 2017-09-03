#include<algorithm>
#include<regex>
#include <sys/socket.h>
#include"Request.h"
#include"HttpCode.h"
#include"Config.h"

thread_local std::string Request::recvData;
Request::Request(Data&data)
{
	recvData.assign(data.buf.begin(),data.buf.end());
}

std::string Request::getMethod() const
{
	return method.str();
}

const std::string& Request::getUrl()const
{
	return url;
}

std::string Request::getHeader(const std::string & s) const
{
	auto p = find_if(head.begin(), head.end(), [&s](const decltype(head)::value_type&t)->bool {
		return t.first == s;
	});
	if (p != head.end())
		return p->second.str();
	else
		return std::string();
}

std::string Request::getArg(const std::string & s) const
{
	auto p = find_if(arg.begin(), arg.end(), [&s](const decltype(arg)::value_type&t)->bool {
		return t.first == s;
	});
	if (p != arg.end())
		return p->second.str();
	else
		return std::string();
}

const std::string& Request::getFileType() const
{
	return fileType;
}

bool Request::analysis(HttpCode&httpCode)
{
	std::smatch m;
	std::regex r(R"(^([^ ]+) (/(?:[^/ \r\n]+/)*(?:[^ /.?\r\n]+(?:\.([^? ]+))?)?)(?:\?([^ ]*))? HTTP/1\.(?:0|1)\r\n([\w\W]*)\r\n([\w\W]*)$)");
	if (std::regex_match(recvData, m, r)) {
		if (m[1] == "POST" || m[1] == "GET") {

			method=m[1];
			fileType.assign(m[3]);
			url.assign(m[2]);

			if (url.back() == '/') {
				url.append("index.html");
				fileType.assign("html");
			}
			else if (fileType.empty())
				fileType.assign("cgi");

			std::regex rt(R"((.+): (.+)\r\n)");
			for (std::sregex_iterator i(m[5].first, m[5].second, rt); i != std::sregex_iterator(); ++i)
				head.emplace_back((*i)[1], (*i)[2]);

			std::smatch::value_type mt;
			if (m[1] == "POST")
				mt = m[6];
			else if (m[1] == "GET")
				mt = m[4];
			std::regex r4(R"(([^?=]+?)=([^&]*)&?)");
			for (std::sregex_iterator i(mt.first, mt.second, r4); i != std::sregex_iterator(); ++i)
				arg.emplace_back((*i)[1], (*i)[2]);
			return true;
		}
		httpCode.setCode(501);
	}
	else
		httpCode.setCode(400);
	return false;
}

bool Request::filter()
{
	return true;
}

bool Request::redirect(HttpCode & httpCode)
{
	if (true||fileType == "html" || fileType == "csp" || fileType == "cgi") {
		if (httpCode.getCode() == 404)
			url = "/error/404.html";
		else if (httpCode.getCode() == 400)
			url = std::string(ERROR_FILE_PATH) += "/error/400.html";
		else if (httpCode.getCode() == 501)
			url = std::string(ERROR_FILE_PATH) += "/error/501.html";
		fileType = "html";
		return true;
	}
	return false;
}
