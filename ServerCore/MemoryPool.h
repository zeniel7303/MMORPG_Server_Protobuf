#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*----------------------------
		 MemoryHeader
------------------------------*/

// 디버깅을 돕는 용도

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	
	MemoryHeader(int32 _size) : allocSize(_size) {}

	static void* AttachHeader(MemoryHeader* _header, int32 _size)
	{
		new(_header)MemoryHeader(_size); // placement new
		return reinterpret_cast<void*>(++_header); // [Data] 반환
	}

	static MemoryHeader* DetachHeader(void* _ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(_ptr) - 1;
		return header;
	}

	int32 allocSize;

	// TODO - 필요한 추가 정보
};

/*----------------------------
		  MemoryPool
------------------------------*/

// 21년 6월 기준
// - Windows 쪽은 메모리 풀링을 안해도 괜찮다. 
// (버전이 올라가며 메모리 파편화 관련된 문제들이 많이 개선됨)
// - Linux 쪽은 여전히 느려 메모리 풀을 사용

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
private:
	SLIST_HEADER	m_header;

	int32			m_allocSize = 0;
	atomic<int32>	m_useCount = 0;
	atomic<int32>	m_reservedCount = 0;

	// 락을 거는 것과 임시로 저장해두는 queue가 있는 것은 썩 좋지 못한 방법.
	// 메모리 할당은 굉장히 자주 일어나는 행위이기때문
	/*USE_LOCK;
	queue<MemoryHeader*> m_queue;*/

public:
	MemoryPool(int32 _allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* _ptr);
	MemoryHeader*	Pop();
};