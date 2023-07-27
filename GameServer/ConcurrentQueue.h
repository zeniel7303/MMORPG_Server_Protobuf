#pragma once
#include <mutex>
template<typename T>
class LockQueue
{
private:
	queue<T> m_queue;
	mutex m_mutex;
	condition_variable m_conVar;

public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T _value)
	{
		lock_guard<mutex> lock(m_mutex);

		m_queue.push(std::move(_value));
		m_conVar.notify_one();
	}

	void WaitPop(T& _value)
	{
		unique_lock<mutex> lock(m_mutex);
		m_conVar.wait(lock, [this] {return m_queue.empty() == false; });

		_value = std::move(m_queue.front());
		m_queue.pop();
	}

	bool TryPop(T& _value)
	{
		lock_guard<mutex> lock(m_mutex);

		if (m_queue.empty()) return false;

		// empty -> top -> pop
		_value = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}
};

/*
멀티 쓰레드 미고려한 러프한 락프리큐

template<typename T>
class LockFreeQueue
{
	struct Node
	{
		shared_ptr<T> data;
		Node* next = nullptr;
	};

private:
	// [data][data][data][data][dummy]
	// [head]            [tail]

	// [dummy]
	// [head] [tail]
	Node* m_head;
	Node* m_tail;

public:
	LockFreeQueue() : m_head(new Node), m_tail(m_head)
	{

	}

	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator=(const LockFreeQueue&) = delete;

	void Push(const T& _value)
	{
		shared_ptr<T> newData = make_shared<T>(_value);
		
		Node* dummy = new Node();

		Node* oldTail = m_tail;
		oldTail->data->swap(newData);
		oldTail->next = dummy;

		m_tail = dummy;
	}
	
	shared_ptr<T> TryPop()
	{
		Node* oldHead = PopHead();
		if (oldHead == nullptr) return shared_ptr<T>();

		shared_ptr<T> res(oldHead->data);
		delete oldHead;
		return res;
	}

private:
	Node* PopHead()
	{
		Nod* oldHead = m_head;
		if (oldHead == m_tail) return nullptr;

		m_head = oldHead->next;
		return oldHead;
	}
};
*/

template<typename T>
class LockFreeQueue
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount; // 참조권
		Node* ptr = nullptr;
	};

	struct NodeCounter	//32bit
	{
		uint32 internalCount : 30;			// 참조권 반환 관련
		uint32 externalCountRemaining : 2;	// Push& Pop 다중 참조권 관련
	};

	struct Node
	{
		Node()
		{
			NodeCounter newCount;
			newCount.internalCount = 0;
			newCount.externalCountRemaining = 2;
			count.store(newCount);

			next.ptr = nullptr;
			next.externalCount = 0;
		}

		void ReleaseRef()
		{
			NodeCounter oldCounter = count.load();
			
			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.internalCount--;

				// 끼어들 수 있음
				if (count.compare_exchange_strong(oldCounter, newCounter))
				{
					if(newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete this;

					break;
				}
			}
		}

		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

private:
	atomic<CountedNodePtr> m_head;
	atomic<CountedNodePtr> m_tail;

public:
	LockFreeQueue()
	{
		CountedNodePtr node;
		node.ptr = new Node;
		node.externalCount = 1;
		
		m_head.store(node);
		m_tail.store(node);
	}

	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator=(const LockFreeQueue&) = delete;

	void Push(const T& _value)
	{
		unique_ptr<T> newData = make_unique<T>(_value);

		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = m_tail.load();	//data = nullptr

		while (true)
		{
			// 참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			IncreaseExternalCount(m_tail, oldTail);

			// 소유권 획득 (data를 먼저 교환한 애가 이김)
			T* oldData = nullptr;
			if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get()))
			{
				oldTail.ptr->next = dummy;
				oldTail = m_tail.exchange(dummy);
				FreeExternalCount(oldTail);
				newData.release();	// 데이터에 대한 unique_ptr의 소유권 포기
				break;
			}

			// 소유권 경쟁 패배
			oldTail.ptr->ReleaseRef();
		}
	}

	shared_ptr<T> TryPop()
	{
		// [data][data][data][]
		// [head][tail]

		CountedNodePtr oldHead = m_head.load();

		while (true)
		{
			// 참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			IncreaseExternalCount(m_head, oldHead);

			Node* ptr = oldHead.ptr;
			if (ptr == m_tail.load().ptr)
			{
				ptr->ReleaseRef();
				return shared_ptr<T>();
			}

			// 소유권 획득 (head = ptr->next)
			if (m_head.compare_exchange_strong(oldHead, ptr->next))
			{
				T* res = ptr->data.load();
				FreeExternalCount(oldHead);
				return shared_ptr<T>(res);
			}

			// 소유권 경쟁 패배
			ptr->ReleaseRef();
		}
	}

private:
	static void IncreaseExternalCount(atomic<CountedNodePtr>& _counter, CountedNodePtr& _oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = _oldCounter;
			newCounter.externalCount++;
			
			if (_counter.compare_exchange_strong(_oldCounter, newCounter))
			{
				_oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

	static void FreeExternalCount(CountedNodePtr& _oldNodePtr)
	{
		Node* ptr = _oldNodePtr.ptr;
		const int32 countIncrease = _oldNodePtr.externalCount - 2;

		NodeCounter oldCounter = ptr->count.load();

		while (true)
		{
			NodeCounter newCounter = oldCounter;
			newCounter.externalCountRemaining--;	// TODO
			newCounter.internalCount += countIncrease;

			if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
			{
				if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
					delete ptr;

				break;
			}
		}
	}
};