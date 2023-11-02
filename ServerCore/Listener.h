#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

/*----------------------------
		   4. Listener
------------------------------*/

// IocpCore에 등록하기위해 IocpObject 상속
class Listener : public IocpObject
{
protected:
	SOCKET					m_socket = INVALID_SOCKET;
	Vector<AcceptEvent*>	m_acceptEvents;
	ServerServiceRef		m_service;

public:
	Listener()				= default;
	~Listener();

public:
	/* 외부에서 사용 */
	bool					StartAccept(ServerServiceRef _service);	// 순서 1
	void					CloseSocket();

public:
	/* 인터페이스 구현 */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;

private:
	/* 수신 관련 */
	void					RegisterAccept(AcceptEvent* _acceptEvent);
	void					ProcessAccept(AcceptEvent* _acceptEvent);
};