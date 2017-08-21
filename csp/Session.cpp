#include <unistd.h>
#include"Session.h"
#include"Request.h"
#include"Response.h"
#include"HttpCode.h"

Session::Session()
{
}

void Session::operator()(int so)
{
	enum class Status { RECVIVE, ANALYSIS, FILTER, REDIRECT, GETFILE, SEND, CLOSE};

	Request request;
	Response response;
	HttpCode httpCode;
	Status s = Status::RECVIVE;

	while (true)
		switch (s) {
		case Status::RECVIVE:
			s = request.recvive(so) ? Status::ANALYSIS : Status::CLOSE;
			break;
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
			response.send(so,httpCode); s = Status::CLOSE;
			break;
		case Status::CLOSE:
			close(so);
			return;
		}
}
