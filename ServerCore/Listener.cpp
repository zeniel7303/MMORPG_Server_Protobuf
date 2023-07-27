#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

/*----------------------------
		   Listener
------------------------------*/

Listener::~Listener()
{
	SocketUtils::Close(m_socket);

	for (AcceptEvent* acceptEvent : m_acceptEvents)
	{
		// TODO

		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef _service)
{
	m_service = _service;
	if (m_service == nullptr)
		return false;

	m_socket = SocketUtils::CreateSocket();
	if (m_socket == INVALID_SOCKET)
		return false;

	if (m_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(m_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(m_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(m_socket, m_service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(m_socket) == false)
		return false;

	const int32 acceptCount = m_service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		// IocpObject의 public enable_shared_from_this 참고
		acceptEvent->m_owner = shared_from_this();
		m_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(m_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(m_socket);
}

void Listener::Dispatch(IocpEvent* _iocpEvent, int32 _numOfBytes)
{
	ASSERT_CRASH(_iocpEvent->m_eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(_iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* _acceptEvent)
{
	SessionRef session = m_service->CreateSession(); // Register IOCP

	_acceptEvent->Init();
	_acceptEvent->m_session = session;

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(m_socket,
		session->GetSocket(), session->m_recvBuffer.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		OUT & bytesReceived, static_cast<LPOVERLAPPED>(_acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(_acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->m_session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), m_socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(),
		OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();

	RegisterAccept(acceptEvent);
}