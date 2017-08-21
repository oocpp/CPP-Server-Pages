#pragma once
#include<queue>
#include<map>
#include<atomic>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<utility>
#include<algorithm>
#include<chrono>
#include"DebugPrint.h"

template<typename Op,typename Arg>
class ThreadPool
{
public:
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	ThreadPool(int i = 5);
	~ThreadPool();

	void addTask(const Arg&t);
private:
	std::mutex mu;
	std::mutex mu_mit;
	std::condition_variable cv;
	void threadRuntime();
	
	std::queue<Arg>qt;
	std::map<std::thread::id, std::thread>mit;
	std::atomic<size_t> isQuit;
	std::atomic<size_t>free_thread_count;

	void SubThreadCount();
	void AddThreadCount();
	size_t threadCount();
	size_t freeThreadCount()const;
	size_t taskCount();

	std::atomic<bool>watch_b;
	void watch();
	std::thread wa_th;
};

template<typename Op, typename Arg>
ThreadPool<Op, Arg>::ThreadPool(int i) :isQuit(false), free_thread_count(0)
{
	watch_b.store(true);
	wa_th = std::thread(&ThreadPool::watch, this);
	while (i--) {
		AddThreadCount();
	}
	//mDebug << "---class ThreadPool constructor" << mendl;
}

template<typename Op, typename Arg>
ThreadPool<Op, Arg>::~ThreadPool()
{
	watch_b.store(false);
	wa_th.join();
	{
		std::lock_guard<std::mutex>l(mu_mit);
		isQuit.store(mit.size());
	}
	cv.notify_all();
}

template<typename Op, typename Arg>
void ThreadPool<Op, Arg>::SubThreadCount()
{
	++isQuit;
	cv.notify_one();
}

template<typename Op, typename Arg>
void ThreadPool<Op, Arg>::AddThreadCount()
{
	std::pair<std::thread::id, std::thread>pt(std::thread::id(), std::thread(&ThreadPool::threadRuntime, this));
	pt.first = pt.second.get_id();
	std::lock_guard<std::mutex>l(mu_mit);
	mit.insert(std::move(pt));
}

template<typename Op, typename Arg>
size_t ThreadPool<Op, Arg>::threadCount()
{
	std::lock_guard<std::mutex>l(mu_mit);
	return mit.size();
}

template<typename Op, typename Arg>
size_t ThreadPool<Op, Arg>::freeThreadCount() const
{
	return free_thread_count.load();
}

template<typename Op, typename Arg>
size_t ThreadPool<Op, Arg>::taskCount()
{
	std::lock_guard <std::mutex>l(mu);
	return qt.size();
}

template<typename Op, typename Arg>
void ThreadPool<Op, Arg>::watch()
{
	while (watch_b.load()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if (freeThreadCount() > 5)
			SubThreadCount();
		else if (taskCount() > 5)
			AddThreadCount();
	}
}

template<typename Op, typename Arg>
void ThreadPool<Op, Arg>::addTask(const Arg & t)
{
	{
		std::lock_guard<std::mutex>lo(mu);
		qt.push(t);
	}
	cv.notify_one();
}

template<typename Op, typename Arg>
void ThreadPool<Op, Arg>::threadRuntime() {
	Op op;
	Arg arg;

	while (true) {
		++free_thread_count;
		{
			std::unique_lock<decltype(mu)>ul(mu);
			cv.wait(ul, [this]()->bool {
				return !qt.empty() || isQuit.load();
			});

			if (isQuit.load()) {
				--isQuit;
				break;
			}
			arg = std::move(qt.front());
			qt.pop();
		}
		--free_thread_count;
		dout << "线程:" << std::this_thread::get_id() << "执行任务";
		op(arg);
	}
	--free_thread_count;

	std::lock_guard<std::mutex>l(mu_mit);
	mit[std::this_thread::get_id()].detach();
	mit.erase(std::this_thread::get_id());
}
