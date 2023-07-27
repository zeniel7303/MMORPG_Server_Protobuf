#pragma once
#include "Types.h"
#include "MemoryPool.h"

template <typename Type>
class ObjectPool
{
private:
	static int32		s_allocSize;
	static MemoryPool	s_pool;

public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(
			StompAllocator::Alloc(s_allocSize));
		Type* memory = static_cast<Type*>(
			MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		Type* memory = static_cast<Type*>(
			MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
		new(memory)Type(forward<Args>(args)...); // placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();

#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}

	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... _args)
	{
		shared_ptr<Type> sptr = { Pop(std::forward<Args>(_args)...), Push };
		return sptr;
	}
};

template <typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template <typename Type>
MemoryPool ObjectPool<Type>::s_pool { s_allocSize };