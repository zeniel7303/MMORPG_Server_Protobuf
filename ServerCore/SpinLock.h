#pragma once
#include <thread>
#include <atomic>

using namespace std;

class SpinLock
{
private:
	atomic<bool> m_locked = false;

public:
	void lock()
	{
		bool expected = false;
		bool desired = true;

		/*if (m_locked == expected)
		{
			expected = m_locked;
			m_locked = desired;
			return true;
		}
		else
		{
			expected = m_locked;
			return false;
		}*/

		while (m_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;

			// Sleep Lock
			//this_thread::sleep_for(100ms);
			//this_thread::yield(); // = this_thread::sleep_for(0ms);
		}
	}

	void unlock()
	{
		m_locked.store(false);
	}
};