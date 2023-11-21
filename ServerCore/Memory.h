#pragma once
#include "Allocator.h"

class MemoryPool;

/*----------------------------
			Memory
------------------------------*/

// MemoryPool을 총괄하는 매니저
class Memory
{
	enum
	{
		// 0 ~ 1024바이트까지는 32바이트 단위
		// ~ 2048바이트까지는 128 바이트 단위
		// ~ 4096바이트까지는 256 바이트 단위로
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

private:
	vector<MemoryPool*> m_pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 빠르게 찾기 위한 헬퍼 테이블
	MemoryPool* m_poolTable[MAX_ALLOC_SIZE + 1];

public:
	Memory();
	~Memory();

	void*	Allocate(int32 _size);
	void	Release(void* _ptr);
};

// 원하는 방식으로 할당하는 것이 목표
// - 메모리 할당 방식(풀링 등) 및 메모리 해제 여부 추적
#pragma region custom new, delete

//						생성자에 들어갈 인자값(들) - Variadic template
template <typename Type, typename... Args>
Type* xnew(Args&&... _args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new 문법
	// 메모리 위에다가 생성자 호출
	new(memory)Type(std::forward<Args>(_args)...);

	return memory;
}

template <typename Type>
void xdelete(Type* _obj)
{
	// 소멸자 호출
	_obj->~Type();
	PoolAllocator::Release(_obj);
}

#pragma endregion custom new, delete

template <typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... _args)
{
	return shared_ptr<Type> {xnew<Type>(std::forward<Args>(_args)...), xdelete<Type>};
}