#pragma once
#include <map>
#include <vector>

/*----------------------------
		DeadLockProfiler
------------------------------*/

class DeadLockProfiler
{
private:
	Mutex								m_lock;

	unordered_map<const char*, int32>	m_nameToId;
	unordered_map<int32, const char*>	m_idToName;
	map<int32, set<int32>>				m_lockHistory;

private:
	// 노드가 발견된 순서를 기록하는 배열
	vector<int32>						m_discoveredOrder;
	// 노드가 발견된 순서
	int32								m_discoveredCount = 0;
	// Dfs(i)가 종료 되었는지 여부
	vector<bool>						m_finished;
	vector<int32>						m_parent;

public:
	void PushLock(const char* _name);
	void PopLock(const char* _name);
	void CheckCycle();

private:
	void Dfs(int32 _index);
};

