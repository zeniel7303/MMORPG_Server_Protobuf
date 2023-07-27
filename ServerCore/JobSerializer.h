#pragma once
#include "Job.h"
#include "JobQueue.h"

/*----------------------------
		 JobSerializer
------------------------------*/

class JobSerializer : public enable_shared_from_this<JobSerializer>
{
protected:
	JobQueue m_jobQueue;

public:
	void PushJob(CallbackType&& _callback)
	{
		auto job = ObjectPool<Job>::MakeShared(std::move(_callback));
		m_jobQueue.Push(job);
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* _memFunc)(Args...), Args... _args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		auto job = ObjectPool<Job>::MakeShared(owner, _memFunc, std::forward<Args>(_args)...);
		m_jobQueue.Push(job);
	}

	virtual void FlushJob() abstract;
};