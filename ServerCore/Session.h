#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*----------------------------
			Session
------------------------------*/

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

private:
	weak_ptr<Service>		m_service;
	SOCKET					m_socket = INVALID_SOCKET;
	NetAddress				m_netAddress = {};
	Atomic<bool>			m_connected = false;

private:
	USE_LOCK;

	/* 수신 관련 */
	RecvBuffer				m_recvBuffer;

	/* 송신 관련 */
	Queue<SendBufferRef>	m_sendQueue;
	Atomic<bool>			m_sendRegistered = false;

private:
	/* IocpEvent 재사용 */
	ConnectEvent			m_connectEvent;
	DisconnectEvent			m_disconnectEvent;
	RecvEvent				m_recvEvent;
	SendEvent				m_sendEvent;

public:
	Session();
	virtual ~Session();

public:
						/* 외부에서 사용 */
	void				Send(SendBufferRef _sendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* _cause);

	shared_ptr<Service>	GetService() { return m_service.lock(); }
	void				SetService(shared_ptr<Service> _service) { m_service = _service; }

public:
						/* 정보 관련 */
	void				SetNetAddress(NetAddress _address) { m_netAddress = _address; }
	NetAddress			GetAddress() { return m_netAddress; }
	SOCKET				GetSocket() { return m_socket; }
	bool				IsConnected() { return m_connected; }
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
						/* 인터페이스 구현 */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;

private:
						/* 전송 관련 */
	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 _numOfBytes);
	void				ProcessSend(int32 _numOfBytes);

	void				HandleError(int32 _errorCode);

protected:
						/* 컨텐츠 코드에서 재정의 */
	virtual void		OnConnected() { }
	virtual int32		OnRecv(BYTE* _buffer, int32 _len) { return _len; }
	virtual void		OnSend(int32 len) { }
	virtual void		OnDisconnected() { }
};

/*-----------------
	PacketSession
------------------*/

struct PacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜ID
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* _buffer, int32 _len) sealed;
	virtual void		OnRecvPacket(BYTE* _buffer, int32 _len) abstract;
};