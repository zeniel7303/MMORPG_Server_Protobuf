#pragma once
#include "Allocator.h"

class MemoryPool;

/*----------------------------
			Memory
------------------------------*/

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
	// O(1) 빠르게 찾기 위한 테이블
	MemoryPool* m_poolTable[MAX_ALLOC_SIZE + 1];

public:
	Memory();
	~Memory();

	void*	Allocate(int32 _size);
	void	Release(void* _ptr);
};

template <typename Type, typename... Args>
Type* xnew(Args&&... _args)
{
	//Type* memory = static_cast<Type*>(BaseAllocator::Alloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new
	// 메모리 위에다가 생성자만 호출
	new(memory)Type(std::forward<Args>(_args)...);

	return memory;
}

template <typename Type>
void xdelete(Type* _obj)
{
	_obj->~Type();

	//BaseAllocator::Release(_obj);
	PoolAllocator::Release(_obj);
}

template <typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... _args)
{
	return shared_ptr<Type> {xnew<Type>(std::forward<Args>(_args)...), xdelete<Type>};
}