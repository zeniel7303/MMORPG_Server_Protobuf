#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> _owner, JobRef _job) : m_owner(_owner), m_job(_job)
	{

	}

	// shared_ptr로 두기엔 생명주기때문에 소멸되지 못할 수 있어 weak_ptr로 쓴다.
	weak_ptr<JobQueue>	m_owner;
	JobRef				m_job;
};

struct TimerItem
{
	bool operator<(const TimerItem& _other) const
	{
		return m_executeTick > _other.m_executeTick;
	}

	// 실행될 틱
	uint64 m_executeTick = 0;
	// JobTimer 내부에서만 사용하고 해제할거라 날포인터여도 괜찮을 것 같다.
	JobData* m_jobData = nullptr;
};

/*----------------------------
			JobTimer
------------------------------*/

// TODO : 전역으로 만들기
class JobTimer
{
private:
	USE_LOCK;

	PriorityQueue<TimerItem>	m_items;
	// 한번에 한개만 일감 배분하도록 compare용 Atomic<bool>
	Atomic<bool>				m_distributing = false;

public:
	void			Reserve(uint64 _tick, weak_ptr<JobQueue> _owner, JobRef _job);
	void			Distribute(uint64 _now);
	void			Clear();
};

