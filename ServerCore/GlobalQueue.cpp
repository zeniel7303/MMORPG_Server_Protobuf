#include "pch.h"
#include "GlobalQueue.h"

/*----------------
	GlobalQueue
-----------------*/

GlobalQueue::GlobalQueue()
{

}

GlobalQueue::~GlobalQueue()
{

}

void GlobalQueue::Push(JobQueueRef _jobQueue)
{
	m_jobQueues.Push(_jobQueue);
}

JobQueueRef GlobalQueue::Pop()
{
	return m_jobQueues.Pop();
}