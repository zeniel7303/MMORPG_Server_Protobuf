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

public:
	// 굳이 오른값일 이유는 없어보임. 레퍼런스 카운트 증감 정도는 감수 가능
	//void Push(JobRef&& _job);
	// _pushOnly 추가 이유 : 공평하게 배분하게 하기 위함이다.
	// CPU 코어마다 일을 균등하게 분배해야하는데 JobQueue 특성상 Job에서 또 Job을 실행하다보면
	// 한쪽으로 일감이 몰리기가 쉽다. 그래서 일감을 밀어넣기만 하고
	// 나중에 각자 빼가는 식으로 구현하면 일의 분배가 쉬워져 이 방식을 사용하기로 했다.
	void Push(JobRef _job, bool _pushOnly = false);
	void Execute();
};