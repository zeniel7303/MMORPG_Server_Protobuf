#pragma once

#include <thread>
#include <functional>

/*----------------------------
		ThreadManager
------------------------------*/

class ThreadManager
{
private:
	Mutex			m_lock;
	vector<thread>	m_threads;

public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> _callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();
};

