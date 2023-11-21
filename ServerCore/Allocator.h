#pragma once

/*----------------------------
		 BaseAllocator
------------------------------*/

// Default
// 굳이 만든 이유 : 생성 혹은 삭제 때 Logging, Counting, Memory Leak 추적 테스트용
class BaseAllocator
{
public:
	static void* Alloc(int32 _size);
	static void Release(void* _ptr);
};

/*----------------------------
		 StompAllocator
------------------------------*/

// 언리얼 엔진에서도 마련되어 있음
// 효율적으로 뭔가를 한다기보단 버그를 잡는데 유용하다.
// (메모리 오염 등)

// 단점 : 작은 사이즈를 할당하려해도 무조건 4kb를 할당하므로 비효율적, 느리다.(필요할때만 사용)
// https://3dmpengines.tistory.com/2231
class StompAllocator
{
public:
			// 4kb
	enum { PAGE_SIZE = 0x1000 };

	static void*	Alloc(int32 _size);
	static void		Release(void* _ptr);
};

/*----------------------------
		 PoolAllocator
------------------------------*/

class PoolAllocator
{
public:
	static void*	Alloc(int32 _size);
	static void		Release(void* _ptr);
};

/*----------------------------
		 STLAllocator
------------------------------*/

// 생성자, 소멸자는 stl에서 알아서 해주므로 신경쓸 필요가 없다.

template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	// 인터페이스 제공용
	template <typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t _count)
	{
		const int32 size = static_cast<int32>(_count * sizeof(T));
		//return static_cast<T*>(Xalloc(size));
		// 메모리 풀 사용 유도
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* _ptr, size_t _count)
	{
		//Xrelease(_ptr);
		PoolAllocator::Release(_ptr);
	}

	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};