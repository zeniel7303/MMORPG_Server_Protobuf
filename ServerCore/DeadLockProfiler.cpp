#include "pch.h"
#include "DeadLockProfiler.h"

/*----------------------------
		DeadLockProfiler
------------------------------*/

void DeadLockProfiler::PushLock(const char* _name)
{
	LockGuard guard(m_lock);

	// 아이디를 찾거나 발급한다.
	int32 lockId = 0;

	auto findIt = m_nameToId.find(_name);
	if (findIt == m_nameToId.end())
	{
		lockId = static_cast<int32>(m_nameToId.size());
		m_nameToId[_name] = lockId;
		m_idToName[lockId] = _name;
	}
	else
	{
		lockId = findIt->second;
	}

	// 잡고 있는 락이 있었다면
	if (LLockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다. (싸이클 판별)
		const int32 prevId = LLockStack.top();
		if (lockId != prevId)
		{
			set<int32>& history = m_lockHistory[prevId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* _name)
{
	LockGuard guard(m_lock);

	// 굳이 없어도 되는 버그 예방차원 코드들
	{
		if (LLockStack.empty()) CRASH("MULTIPLE_UNLOCK");

		int32 lockId = m_nameToId[_name];
		if (LLockStack.top() != lockId) CRASH("INVALID_UNLOCK");
	}

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(m_nameToId.size());
	m_discoveredOrder = vector<int32>(lockCount, -1);
	m_discoveredCount = 0;
	m_finished = vector<bool>(lockCount, false);
	m_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);

	// 연산이 끝났으면 정리한다.
	m_discoveredOrder.clear();
	m_finished.clear();
	m_parent.clear();
}

void DeadLockProfiler::Dfs(int32 _index)
{
	if (m_discoveredOrder[_index] != -1)
		return;

	m_discoveredOrder[_index] = m_discoveredCount++;

	// 모든 인접한 정점을 순회한다.
	auto findIt = m_lockHistory.find(_index);
	if (findIt == m_lockHistory.end())
	{
		m_finished[_index] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문한다.
		if (m_discoveredOrder[there] == -1)
		{
			m_parent[there] = _index;
			Dfs(there);
			continue;
		}

		// _here가 there보다 먼저 발견되었다면, there는 _here의 후손이다. (순방향 간선)
		if (m_discoveredOrder[_index] < m_discoveredOrder[there])
			continue;

		// 순방향이 아니다.
		// DFS(there)가 아직 종료하지 않았다면, there는 _here의 선조이다. (역방향 간선)
		if (m_finished[there] == false)
		{
			printf("%s -> %s \n", m_idToName[_index], m_idToName[there]);

			int32 now = _index;
			while (true)
			{
				printf("%s -> %s \n", m_idToName[m_parent[now]], m_idToName[now]);
				now = m_parent[now];

				if (now == there) break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	m_finished[_index] = true;
}