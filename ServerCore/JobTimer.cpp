#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

/*----------------------------
			JobTimer
------------------------------*/

void JobTimer::Reserve(uint64 _tick, weak_ptr<JobQueue> _owner, JobRef _job)
{
	const uint64 executeTick = ::GetTickCount64() + _tick;
	// STOMP 미사용시(Release)엔 이미 내부적으로 Lock이 걸려있다.
	JobData* jobData = ObjectPool<JobData>::Pop(_owner, _job);

	WRITE_LOCK;

	m_items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 _now)
{
	// 한 번에 1 쓰레드만 통과
	// -> 운나쁘게 쓰레드가 여러개 진입해 일처리를 동시에 하게되면
	// 일감의 순서가 꼬일 수도 있다.(재현이 거의 되지 않음)
	if (m_distributing.exchange(true) == true)
		// 이전 값이 true면 누군가 이미 이 함수를 실행 중이니 return
		return;

	// 최소한의 락
	// 실행될 애들만 뽑아두기 위한 지역 변수
	Vector<TimerItem> items;
	{
		WRITE_LOCK;

		while (m_items.empty() == false)
		{
			const TimerItem& timerItem = m_items.top();
			if (_now < timerItem.m_executeTick)
				// 때가 되지 않음
				break;

			// 실행할 때가 됨
			items.push_back(timerItem);
			m_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.m_jobData->m_owner.lock())
			owner->Push(item.m_jobData->m_job);

		ObjectPool<JobData>::Push(item.m_jobData);
	}

	// 끝났으면 풀어준다
	m_distributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;

	while (m_items.empty() == false)
	{
		const TimerItem& timerItem = m_items.top();
		ObjectPool<JobData>::Push(timerItem.m_jobData);
		m_items.pop();
	}
}
