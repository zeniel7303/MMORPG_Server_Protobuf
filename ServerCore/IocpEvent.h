#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	// https://ozt88.tistory.com/26 - PAGE_LOCKING & ZeroByteRecv로 PAGE_LOCKING 최소화
	//PreRecv,		// 경우에 따라 recv 하기 이전에 정의 하는 경우가 있다. (zero-byte receive라 하는 고급기법)
	Recv,
	Send
};

/*----------------------------
		  3. IocpEvent
------------------------------*/

// OVERLAPPED는 상속으로 써도되고 첫번째 맴버변수로 써도된다.
// 상속구조로 하면 첫번째 offset에는 OVERLAPPED 메모리가 들어있을태니 포인터로 사용하기 편할 것
// 주의
// virtual 함수 사용 금지 - virtual 함수때문에 offset 0번 메모리에 가상 함수 테이블이 들어가 OVERLAPPED와 관련된 메모리가 아니라 다른걸로 채워질 수 있다.
// (OVERLAPPED 메모리가 offset 0번 메모리가 되어야한다.)
class IocpEvent : public OVERLAPPED
{
public:
	EventType		m_eventType;
	IocpObjectRef	m_owner;

public:
	IocpEvent(EventType _type);

	void			Init();
	EventType		GetType() { return m_eventType; }
};

/*----------------------------
		  ConnectEvent
------------------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------------------
		DisconnectEvent
------------------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*----------------------------
		  AcceptEvent
------------------------------*/

class AcceptEvent : public IocpEvent
{
public:
	// acceptex의 인자 중에 클라의 소켓을 받아주는 부분이 있어 추가
	SessionRef m_session = nullptr;

public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }
};

/*----------------------------
		  RecvEvent
------------------------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------------------
		  SendEvent
------------------------------*/

class SendEvent : public IocpEvent
{
public:
	Vector<SendBufferRef> m_sendBuffers;

public:
	SendEvent() : IocpEvent(EventType::Send) { }
};