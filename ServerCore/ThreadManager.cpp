#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"

/*----------------------------
		ThreadManager
------------------------------*/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> _callback)
{
	LockGuard guard(m_lock);

	m_threads.push_back(thread([=]()
		{
			InitTLS();
			_callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : m_threads)
	{
		if (t.joinable())
			t.join();
	}
	m_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}
