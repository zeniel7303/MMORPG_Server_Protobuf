#pragma once
/*
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>

using namespace std;

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle);
		this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}*/

//void main()
//{
//	// 커널 오브젝트
//	// Usage Count
//	// Signal(파란불), Non-Signal(빨간불) << bool
//	// Auto , Manual << bool
//	handle = ::CreateEvent(NULL/*보안속성*/, FALSE/*bManualReset*/, FALSE/*bInitialState*/, NULL);
//
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//
//	::CloseHandle(handle);
//}