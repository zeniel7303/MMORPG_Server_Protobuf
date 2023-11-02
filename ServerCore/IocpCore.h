#pragma once

/*----------------------------
		  2. IocpObject
------------------------------*/

// https://chipmaker.tistory.com/entry/enablesharedfromthis-%EC%A0%95%EB%A6%AC
// enable_shared_from_this
// 내부적으로 자기 자신의 weakPtr을 갖고있다.
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) abstract;
};

/*----------------------------
		  1. IocpCore
------------------------------*/

class IocpCore
{
private:
	HANDLE		m_iocpHandle;

public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return m_iocpHandle; }

	// IOCP에 관찰 대상으로 등록
	bool		Register(IocpObjectRef _iocpObject);
	// WorkerThread들이 IOCP에 일거리를 찾는 함수
	bool		Dispatch(uint32 _timeoutMs = INFINITE);
};