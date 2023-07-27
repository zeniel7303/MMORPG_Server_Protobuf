#pragma once

/*----------------------------
			JobQueue
------------------------------*/

class JobQueue
{
private:
	USE_LOCK;
	queue<JobRef> m_jobs;

public:
	void Push(JobRef _job)
	{
		WRITE_LOCK;
		m_jobs.push(_job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (m_jobs.empty())
			return nullptr;

		JobRef ret = m_jobs.front();
		m_jobs.pop();
		return ret;
	}
};
