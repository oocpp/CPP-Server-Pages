#include<regex>
#include<fstream>
#include<iterator>
#include<cstdlib>
#include <dlfcn.h>
#include"GetCPPSP.h"
#include"Config.h"
#include"Request.h"
#include"Response.h"
#include "DebugPrint.h"

bool GetCPPSP::getFile(const std::string & name,std::string& str)
{
	std::ifstream in(std::string(WEB_ROOT_PATH) += name);
	if (in.is_open()) {
		str.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
		in.close();
		return true;
	}
	return false;
}

bool GetCPPSP::makeCpp(std::string & scp,const std::string & st, Response&response)
{
	std::string out1, out2, out3;

	std::regex r(R"(<%([@=!])?([\d\D]+?)%>)");
	std::regex r1(R"A( *(page|using) *(.+)="(.+)" *)A");

	std::sregex_iterator i,j;
	for (i= std::sregex_iterator(st.begin(), st.end(), r); i != std::sregex_iterator(); ++i) {
		if (i->prefix().matched)
			out3.append("out<<R\"(").append(i->prefix()).append(")\";");
		if (!(*i)[1].matched)
			out3.append(i->str(2));
		else if ((i->str(1))[0] == '=')
			out3.append("out<<").append(i->str(2)) += ';';
		else if (i->str(1)[0] == '!')
			out2.append(i->str(2)) += '\n';
		else if (i->str(1)[0] == '@') {
			/*out1 .append(i->str(2))+= '\n';*/
			std::smatch sm;
			if (std::regex_match((*i)[2].first, (*i)[2].second, sm, r1)) {
				if (sm[1] == "using") {
					if (sm[2] == "namespace")
						out1.append("using namespace ").append(sm[3]).append(";\n");
				}
				else if (sm[1] == "page") {
					if (sm[2] == "include")
						out1.append("#include<").append(sm[3]).append(">\n");
					else
						response.setHead(sm[2], sm[3]);
				}
			}
		}
		j = i;
	}
	if (i == std::sregex_iterator()) {
		out3.append("out<<R\"(").append(j->suffix()).append(")\";");


		scp[scp.length() - 2] = 'p';

		std::ofstream out(std::string(WORK_TEMP_ROOT_PATH) += scp);
		std::string head = R"(#include<sstream>
#include<string>
std::ostringstream out;
)";
		std::string be = R"(extern "C" std::string __FUN__(){
)";
		std::string en = R"(return out.str();})";
		out << out1 << head << out2 << be << out3 << en;
		out.close();
		return true;
	}
	return false;
}

bool GetCPPSP::operator()(Request&request, Response&response)
{
	std::string scp = request.getUrl();
	std::string st;

	if (getFile(scp, st) && makeCpp(scp, st, response)) {

		std::string bat = std::string(SCRIPT_PATH).append("MakeDll.sh ") + scp.substr(1, scp.rfind('.') - 1);
		std::system(bat.c_str());

		scp.replace(scp.end() - 3, scp.end(), "so");

		void * pHandle = dlopen((std::string(WORK_ROOT_PATH) += scp).c_str(), RTLD_LAZY);
		if (pHandle != nullptr) {
			typedef std::string(*FUN)();
			FUN fun = (FUN)dlsym(pHandle, "__FUN__");
			if (fun != nullptr)
			{
				response.setSendData(fun());
				dlclose(pHandle);
				return true;
			}
			dlclose(pHandle);
		}
		else{
			dout<<dlerror();
			dlclose(pHandle);
		}
	}
	return false;
}