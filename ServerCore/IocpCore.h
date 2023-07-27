#pragma once

/*----------------------------
		  IocpObject
------------------------------*/

// enable_shared_from_this
// 내부적으로 자기 자신의 weakPtr을 갖고있다.
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) abstract;
};

/*----------------------------
		  IocpCore
------------------------------*/

class IocpCore
{
private:
	HANDLE		m_iocpHandle;

public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return m_iocpHandle; }

	bool		Register(IocpObjectRef _iocpObject);
	bool		Dispatch(uint32 _timeoutMs = INFINITE);
};