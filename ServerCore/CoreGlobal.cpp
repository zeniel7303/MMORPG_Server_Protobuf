#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*		GThreadManager = nullptr;
Memory*				GMemory = nullptr;
SendBufferManager*	GSendBufferManager = nullptr;
GlobalQueue*		GGlobalQueue = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal() 
	{
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GGlobalQueue = new GlobalQueue();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}

	~CoreGlobal() 
	{
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GCoreGlobal;