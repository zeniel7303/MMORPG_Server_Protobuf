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

	// 1개만 할 경우 많은 동접이 있을 때 몇몇은 접속이 안될 가능성이 있으므로 여러개 만든다.
	const int32 acceptCount = m_service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();

		/*이러면 큰일난다.
		이유 : shared_ptr을 처음부터 만들어주고 그 포인터를 this 포인터를 넘겨줬기때문에 실질적으로 RefCount가 1인 shared_ptr을 새로 생성한 셈이 된다.
		그러면 외부에서 이 리스너를 또 다른 shared_ptr로 관리하게된다면 동일한 포인터를 2개의 shared_ptr로 관리하게 되는 것이고,
		혹시라도 아래의 코드가 RefCount가 0이 되어버리면 절대 지우면 안되는 것을 지워버리게될 수 있다.
		-> enable_shared_from_this로 해결
		RawPointer로 shared_ptr을 만드는건 언제나 지양하자.
		acceptEvent->m_owner = shared_ptr<IocpObject>(this);*/

		// IocpObject의 public enable_shared_from_this 참고
		// shared_from_this를 사용했다면 사용한 곳은 shared_ptr로 활용해야한다. -> Listener도 shared_ptr로 관리
		acceptEvent->m_owner = shared_from_this();	// 자기 자신에 대한 RefCount를 유지한 채로 Shared_ptr 생성
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