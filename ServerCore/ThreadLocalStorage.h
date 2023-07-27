#pragma once
/*
#include "pch.h"
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

//__declspec(thread) int32 value;
thread_local int32 LThreadId;
//thread_local queue<int32> q;

void ThreadMain(int32 threadId)
{
	LThreadId = threadId;

	while (true)
	{
		cout << "Hi! I am Thread " << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

void main()
{
	vector<thread> threads;

	for (int32 i = 0; i < 10; i++)
	{
		int32 threadId = i + 1;
		threads.emplace_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
	{
		t.join();
	}
}
*/