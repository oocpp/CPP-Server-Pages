#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <memory.h>
#include "Web.h"
#include "ThreadPool.h"
#include"Session.h"
#include"DebugPrint.h"

Web::Web() noexcept
{
	dout << "---class Web constructor";
}

Web::~Web() noexcept
{
    dout << "---class Web deconstructor";
}

void Web::start() noexcept
{
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	sa.sin_addr.s_addr = INADDR_ANY;
    bzero(&sa.sin_zero,8);

	int so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    dout<<so;

	if (::bind(so, (sockaddr*)&sa, sizeof(sa)) > -1 && listen(so, SOMAXCONN) >  -1) {
        dout<<"进入 web循环";
		ThreadPool<Session,int> tp;
		while (true) {
			tp.addTask(accept(so, nullptr, nullptr));
		}
	}
	
	close(so);
}
