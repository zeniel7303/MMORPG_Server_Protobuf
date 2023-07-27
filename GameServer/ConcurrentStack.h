#pragma once
#include <mutex>

template<typename T>
class LockStack
{
private:
	stack<T> m_stack;
	mutex m_mutex;
	condition_variable m_conVar;

public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T _value)
	{
		lock_guard<mutex> lock(m_mutex);

		m_stack.push(std::move(_value));
		m_conVar.notify_one();
	}

	void WaitPop(T& _value)
	{
		unique_lock<mutex> lock(m_mutex);
		m_conVar.wait(lock, [this] {return m_stack.empty() == false; });

		_value = std::move(m_stack.top());
		m_stack.pop();
	}

	bool TryPop(T& _value)
	{
		lock_guard<mutex> lock(m_mutex);

		if (m_stack.empty()) return false;

		// empty -> top -> pop
		_value = std::move(m_stack.top());
		m_stack.pop();
		return true;
	}

	// 멀티쓰레드는 의미가 없다.
	/*bool Empty()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_stack.empty();
	}*/
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& _value) : data(_value), next(nullptr) {}

		T data;
		Node* next;
	};

private:
	// [ ] [ ] [ ] [ ] [ ]
	// [head]
	atomic<Node*> m_head;

	atomic<int32> m_popCount;		// Pop을 실행중인 쓰레드 갯수
	atomic<Node*> m_pendingList;	// 삭제 되어야 할 노드들 (첫번쨰 노드)

public:
	LockFreeStack()
	{
		m_popCount = 0;
	}

	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;

	// 1) 새 노드를 만들고
	// 2) 새 노드의 next = head
	// 3) head = 새 노드
	void Push(const T& _value)
	{
		Node* node = new Node(_value);
		node->next = m_head;

		/*
		if (m_head == node->next)
		{
			m_head = node;
			return true;
		}
		else
		{
			node->next = m_head;
			return false;
		}
		*/
		while (m_head.compare_exchange_weak(node->next, node) == false)
		{		
		}
	}

	// 1) head 읽기
	// 2) head->next 읽기
	// 3) head = head->next
	// 4) data 추출해서 반환
	// 5) 추출한 노드 삭제
	bool TryPop(T& _value)
	{
		++m_popCount;

		Node* oldHead = m_head;

		/*
		if (m_head == oldHead)
		{
			m_head = oldHead->next;
			return true;
		}
		else
		{
			oldHead = m_head;
			return false;
		}
		*/
		while (oldHead && m_head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{			
		}

		if (oldHead == nullptr)
		{
			--m_popCount;
			return false;
		}

		_value = oldHead->data;
		// C#이나 Java같이 GC가 있으면 사실 여기서 끝이다.
		TryDelete(oldHead);

		return true;
	}

	// 1) 데이터 분리
	// 2) Count 체크
	// 3) 나 혼자면 삭제
	void TryDelete(Node* _oldHead)
	{
		// 나 외에 누가 있는가?
		if (m_popCount == 1)
		{
			// 나 혼자구나

			// 이왕 혼자인거, 삭제 예약된 다른 데이터들도 삭제해보자
			Node* node = m_pendingList.exchange(nullptr);

			if (--m_popCount == 0)
			{
				// 중간에 끼어든 애가 없다 -> 삭제 진행
				// 이제와서 누군가가 끼어들어도 어차피 데이터는 분리해둔 상태이다.			
				DeleteNodes(node);
			}
			else if(node)
			{
				// 누가 끼어들었으니 다시 갖다 놓자
				ChainPendingNodeList(node);
			}

			// 내 데이터는 삭제
			delete _oldHead;
		}
		else
		{
			// 누가 있으니 지금 삭제하지 않고 삭제 예약만
			ChainPendingNode(_oldHead);
			--m_popCount;
		}
	}

	void ChainPendingNodeList(Node* _first, Node* _last)
	{
		_last->next = m_pendingList;

		while (m_pendingList.compare_exchange_weak(_last->next, _first) == false)
		{

		}
	}

	void ChainPendingNodeList(Node* _node)
	{
		Node* last = _node;

		while (last->next) last = last->next;

		ChainPendingNodeList(_node, last);
	}

	void ChainPendingNode(Node* _node)
	{
		ChainPendingNodeList(_node, _node);
	}

	static void DeleteNodes(Node* _node)
	{
		while (_node)
		{
			Node* next = _node->next;
			delete _node;
			_node = next;
		}
	}
};

// 락프리 프로그램을 연습하려 만든것이지 진짜로 락프리가 아니다.
template<typename T>
class LockFreeStack2
{
	struct Node
	{
		Node(const T& _value) : data(make_shared<T>(_value)), next(nullptr) {}

		shared_ptr<T> data;
		shared_ptr<Node> next;
	};

private:
	shared_ptr<Node> m_head;

public:
	LockFreeStack2() {}

	LockFreeStack2(const LockFreeStack2&) = delete;
	LockFreeStack2& operator=(const LockFreeStack2&) = delete;

	void Push(const T& _value)
	{
		shared_ptr<Node> node = make_shared<Node>(_value);
		node->next = std::atomic_load(&m_head);

		while (std::atomic_compare_exchange_weak(&m_head, &node->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&m_head);

		while (oldHead && std::atomic_compare_exchange_weak(&m_head, &oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr) return shared_ptr<T>();

		return oldHead->data;
	}
};

template<typename T>
class LockFreeStack3
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& _value) : data(make_shared<T>(_value)) {}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};

private:
	atomic<CountedNodePtr> m_head;

public:
	LockFreeStack3() {}

	LockFreeStack3(const LockFreeStack3&) = delete;
	LockFreeStack3& operator=(const LockFreeStack3&) = delete;

	void Push(const T& _value)
	{
		CountedNodePtr node;
		node.ptr = new Node(_value);
		node.externalCount = 1;
		// [!]
		node.ptr->next = m_head;
		while (m_head.compare_exchange_weak(node.prt->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = m_head;
		while (true)
		{
			// 참조권 횐득 (externalCount를 현 시점 기준 +1 한 애가 이김)
			IncreaseHeadCount(oldHead);
			// 최소한 externalCount >= 2 일태니 삭제X (안전하게 접근할 수 있는)
			Node* ptr = oldHead.ptr;

			// 데이터 없음
			if (ptr == nullptr)
				return shared_ptr<T>();

			// externalCount 시작값 : 1 -> 2(+1)
			// internalCount 시작값 : 0

			// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)		
			if (m_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				// 나 말고 또 누가 있는가?
				const int32 countIncrease = oldHead.externalCount - 2;

				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if(ptr->internalCount.fetch_sub(1) == 1)
			{
				// 참조권은 얻었으나, 소유권은 실패 -> 뒷수습은 내가 한다.
				delete ptr;
			}
		}
	}

	void IncreaseHeadCount(CountedNodePtr& _oldCounter)
	{
		while(true)
		{
			CountedNodePtr newCounter = _oldCounter;
			newCounter.externalCount++;

			if (m_head.compare_exchange_strong(_oldCounter, newCounter))
			{
				_oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}
};

// MemoryPool 공부용 LockFreeStack
// =====================================================================

/*----------------------------
		   1차 시도
------------------------------*/

/*struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/

/*----------------------------
		   2차 시도
------------------------------*/

/*struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/

/*----------------------------
		   3차 시도
------------------------------*/

// MS에서 만든것을 유사하게 제작
// MS에서 만든것을 사용할 것이다.(사용하지 않을 것)
// LockFree는 직접 구현하기엔 너무 위험하다.

// 무조건 메모리가 16바이트 정렬이 되게끔 컴파일러에게 요청
// 16바이트 정렬로 하게되면 뒤는 하위 4비트는 0000이 된다.

/*DECLSPEC_ALIGN(16)
struct SListEntry
{
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	// ABA 문제 해결을 위함
	// 두가지 시각으로 동일한 데이터를 접근
	union
	{
		struct
		{
			// 128 비트
			uint64 alignment;
			uint64 region;
		} DUMMYSTRUCTNAME;
		struct
		{
			// 일종의 카운터 개념
			// uint64 alignment;
			uint64 depth		: 16;
			uint64 sequence		: 48; 
			// uint64 region;
			uint64 reserved		: 4;
			uint64 next			: 60;
		} HeaderX64;
	};
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/