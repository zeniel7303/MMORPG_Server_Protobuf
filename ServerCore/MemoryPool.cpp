#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 _allocSize) : m_allocSize(_allocSize)
{
	::InitializeSListHead(&m_header);
}

MemoryPool::~MemoryPool()
{
	/*while (m_queue.empty() == false)
	{
		MemoryHeader* header = m_queue.front();
		m_queue.pop();
		::free(header);
	}*/

	while (MemoryHeader* memory = static_cast<MemoryHeader*>(
		::InterlockedPopEntrySList(&m_header)))
	{
		// null이 아니면
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* _ptr)
{
	//WRITE_LOCK;
	_ptr->allocSize = 0;

	// Pool에 메모리 반납
	//m_queue.push(_ptr);
	::InterlockedPushEntrySList(&m_header, 
		static_cast<PSLIST_ENTRY>(_ptr));

	//m_allocSize.fetch_sub(1);
	m_useCount.fetch_sub(1);
	m_reservedCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(
		::InterlockedPopEntrySList(&m_header));

	/*MemoryHeader* header = nullptr;
	
	{
		WRITE_LOCK;

		//Pool에 여분이 있는지?
		if (m_queue.empty() == false)
		{
			//있으면 하나 꺼내온다.
			header = m_queue.front();
			m_queue.pop();
		}
	}*/

	// 없으면 새로 할당
	if (memory == nullptr)
	{
		//header = reinterpret_cast<MemoryHeader*>(::malloc(m_allocSize));

		memory = reinterpret_cast<MemoryHeader*>(
			::_aligned_malloc(m_allocSize, SLIST_ALIGNMENT));
	}
	// 여분 데이터가 있다.
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		m_reservedCount.fetch_sub(1);
	}

	//m_allocSize.fetch_add(1);
	m_useCount.fetch_add(1);

	return memory;
}
