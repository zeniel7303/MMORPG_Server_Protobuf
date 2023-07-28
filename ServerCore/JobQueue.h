#pragma once
#include "Job.h"
#include "LockQueue.h"

/*----------------------------
			JobQueue
------------------------------*/

class JobQueue : public enable_shared_from_this<JobQueue>
{
protected:
	LockQueue<JobRef>	m_jobs;
	// 일감 추적용
	Atomic<int32>		m_jobCount;

public:
	void DoAsync(CallbackType&& _callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(_callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* _memFunc)(Args...), Args... _args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, _memFunc, std::forward<Args>(_args)...));
	}

	void ClearJobs() { m_jobs.Clear(); }

private:
	void Push(JobRef&& _job);
	void Execute();
};