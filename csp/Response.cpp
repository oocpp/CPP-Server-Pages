#include "Response.h"
#include"Request.h"
#include"HttpCode.h"
#include"GetCPPSP.h"
#include"Config.h"
#include<fstream>
#include<iterator>
#include <sys/socket.h>
#include"TaskQueue.h"

Response::Response(TaskQueue&tq):_queue(tq)
{
	response.emplace("Server", SERVER_NAME);
}


Response::~Response()
{
}

void Response::setHead(std::string&&name,std::string &&value) {
	response.emplace(std::forward<std::string>(name), std::forward<std::string>(value));
}

bool Response::getFile(Request & request, HttpCode & httpCode)
{
	if (request.getFileType() == "csp") {

		if (GetCPPSP()(request, *this)) {
			httpCode.setCode(200);
			return true;
		}
			httpCode.setCode(404);
	}
	else if (request.getFileType() == "cgi") {
		httpCode.setCode(404);
		return false;
	}
	else {

		if(request.getFileType()=="html")
			setHead("Content-Type", "text/html");

		std::ifstream fin;
		fin.open(std::string(WEB_ROOT_PATH).append(request.getUrl()));

		if (fin.is_open()) {
			sendData.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
			httpCode.setCode(200);
			return true;
		}
		else
			httpCode.setCode(404);
	}
	return false;
}

bool Response::send(Buffer&data,HttpCode & httpCode)
{
	std::string head = std::string("HTTP/1.1 ").append(httpCode.getCodeStr()).append("\r\n");
	response.emplace("Content-Length", std::to_string(sendData.length()));

	std::string str;
	for (auto &i : response)
		str.append(i.first).append(": ").append(i.second).append("\r\n");
	str += "\r\n";

	std::string ss= head +str+sendData;

	data.setSendData(ss);
	_queue.addWriteTask(data);
    dout<<"send";
	//_queue.notifyWrite();
	return true;
}

void Response::setSendData(std::string && s)
{
	sendData = std::forward<std::string>(s);
}
