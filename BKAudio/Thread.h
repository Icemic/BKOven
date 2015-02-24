#pragma once

#include "Common.h"
#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>

class  BKE_Thread
{
	std::thread *_thread;
public:
	BKE_Thread():_thread(NULL){}
	~BKE_Thread()
	{ 
		if (_thread)
		{
			if (_thread->joinable())
				_thread->join();
			delete _thread;
		}
	}
	template<class F, class...A>
	void call(F f, A... a){ _thread = new std::thread(f, a...); }
	void join()
	{ 
		if (_thread)
		{
			_thread->join();
			delete _thread;
			_thread = NULL;
		}
	}
	static inline std::thread::id GetThreadId(){
		return std::this_thread::get_id();
	}
	static void sleep(int ms){
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
};

class BKE_Notify 
{
public:
	BKE_Notify(int value = 0) : count{ value }{}

	void wait(){
		std::unique_lock<std::mutex> lock{ mutex };
		if (--count < 0) // count is not enough ?
			condition.wait(lock); // suspend and wait...
	}
	void set(){
		std::lock_guard<std::mutex> lock{ mutex };
		if (++count <= 0) // have some thread suspended ?
			condition.notify_one(); // notify one !
	}
	void init(){}

private:
	int count;
	std::mutex mutex;
	std::condition_variable condition;
};

class BKE_Mutex
{
	std::recursive_mutex _mutex;
public:
	void lock(){ _mutex.lock(); }
	void unlock(){ _mutex.unlock(); }
};

class BKE_MutexLocker{
	BKE_Mutex &mutex;
	BKE_MutexLocker(const BKE_MutexLocker &m):mutex(m.mutex){}
	void operator=(const BKE_MutexLocker &){}
public:
	BKE_MutexLocker(BKE_Mutex &m):mutex(m){
		mutex.lock();
	}
	~BKE_MutexLocker(){
		mutex.unlock();
	}
};


class BKE_Clock{
#ifdef BKE_SYS_WINDOWS
	bkulonglong data;
#endif
public:
	typedef double t;
	static t Max;
#ifdef BKE_SYS_WINDOWS
	BKE_Clock();
#endif
	t get() const;
};
