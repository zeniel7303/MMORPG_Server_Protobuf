#pragma once

/*----------------------------
		 RefCountable
------------------------------*/

class RefCountable
{
protected:
	atomic<int32> m_refCount;

public:
	RefCountable() : m_refCount(1) {}
	virtual ~RefCountable() {}

	int32 AddRef() { return ++m_refCount; }
	int32 GetRefCount() { return m_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --m_refCount;
		if (refCount == 0)
		{
			delete this;	
		}

		return refCount;
	}
};

/*----------------------------
		   SharedPtr
------------------------------*/

// 1) 이미 만들어진 클래스 대상으로 사용 불가
// - RefCountable을 상속받아야하므로 외부 라이브러리 사용 시 사용 불가
// 2) 순환(Cycle) 참조 문제 (표준 shared_ptr도 같은 문제)

template <typename T>
class TSharedPTr
{
private:
	T* m_ptr = nullptr;

public:
	TSharedPTr() {}
	TSharedPTr(T* _ptr) { Set(_ptr); }
	// 복사
	TSharedPTr(const TSharedPTr& _rhs) { Set(_rhs.m_ptr); }
	// 이동
	TSharedPTr(TSharedPTr&& _rhs) 
	{ 
		m_ptr = _rhs.m_ptr; 
		_rhs.m_ptr = nullptr; 
	}
	// 상속 관계 복사
	template<typename U>
	TSharedPTr(const TSharedPTr<U>& _rhs) 
	{ 
		Set(static_cast<T*>(_rhs.m_ptr)); 
	}

	~TSharedPTr() { Release(); }

public:
	// 복사 연산자
	TSharedPTr& operator=(const TSharedPTr& _rhs)
	{
		if (m_ptr != _rhs.m_ptr)
		{
			Release();
			Set(_rhs.m_ptr);
		}
		return *this;
	}
	// 이동 연산자
	TSharedPTr& operator=(TSharedPTr&& _rhs)
	{
		Release();
		m_ptr = _rhs.m_ptr;
		_rhs.m_ptr = nullptr;
		return *this;
	}
	// 비교
	bool		operator==(const TSharedPTr& _rhs)	{ return m_ptr == _rhs.m_ptr; }
	bool		operator==(T* _ptr) const			{ return m_ptr == _ptr; }
	bool		operator!=(const TSharedPTr& _rhs)	{ return m_ptr != _rhs.m_ptr; }
	bool		operator!=(T* _ptr) const			{ return m_ptr != _ptr; }
	// 대소관계
	bool		operator<(const TSharedPTr& _rhs)	{ return m_ptr < _rhs.m_ptr; }
	
	T*			operator*()							{ return m_ptr; }
	const T*	operator*() const					{ return m_ptr; }
	// Conversion Operator or Casting Operator
	// 다른 클래스로 캐스팅할 때 적용됨
				operator T* () const				{ return m_ptr; }
	T*			operator->()						{ return m_ptr; }
	const T*	operator->() const					{ return m_ptr; }

	bool isNull() { return m_ptr == nullptr; }

private:
	inline void Set(T* _ptr)
	{
		m_ptr = _ptr;
		if (m_ptr)
			_ptr->AddRef();
	}

	inline void Release()
	{
		if (m_ptr != nullptr)
		{
			m_ptr->ReleaseRef();
			m_ptr = nullptr;
		}
	}
};

// unique_ptr
// - 기존 포인터와 차이가 없다.
// - 이동만 가능하고 복사하는 기능은 막혀있다. (유니크해야한다.)

// [T*][RefCountBlock*]
// shared_ptr
// weak_ptr
// - shared_ptr의 순환 문제 해결 가능
// - expired()로 존재하는지 체크하고 사용해야함
// - 혹은 lock()을 이용해 shared_ptr로 다시 캐스팅해 사용한다.
// - 내부에 RefCountBlock에는 useCount와 weakCount가 있다.
//	 useCount는 지금까지 알던 refCount다. 0이 되면 유효하지 않으므로 삭제
//   weakCount는 weak_ptr 사용 체크용 refCount다.
//   고로 useCount와 weakCount 둘 다 체크해서 해당 스마트 포인터를 제거할지 확인한다.