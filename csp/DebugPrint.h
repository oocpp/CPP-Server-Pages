#pragma once
#include<sstream>
#include<cstdio>
#include<cstdarg>

#ifdef _DEBUG
#include<string>
#endif // _DEBUG

class __My_Debug {
public:
	__My_Debug(const char*file, int line, const char*func) {

#ifdef _DEBUG	/*防止debug模式下,,文件名前带有路径*/
		file += std::string(file).rfind('\\') + 1;
#endif // _DEBUG

		/*输出前缀内容 file文件名,line 行号,所在函数名*/
		this->out << file << '(' << line << ") " << func << " >> ";
	}

	template<typename T>
	__My_Debug&operator<<(T const& t) {
		this->out << t;
		return *this;
	}

	__My_Debug&operator<<(std::ostream&(*op)(std::ostream&)) {
		op(this->out);
		return *this;
	}

	__My_Debug& operator()(const char*s, ...) {
		char buf[1024];/*注意 最大输出长度1024..自行设置*/
		va_list ap;
		va_start(ap, s);
		int n = vsnprintf(buf, 1024, s, ap);
		va_end(ap);
		buf[n] = '\0';
		out << buf;
		return *this;
	}

	~__My_Debug();

private:
	std::stringstream out;
};

/*__FUNCTION__ 可以替换成 __func__,看ide提供那个*/
#define dout __My_Debug(__FILE__,__LINE__,__FUNCTION__)