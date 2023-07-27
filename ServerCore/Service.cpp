#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

/*-------------
	Service
--------------*/

Service::Service(ServiceType _type, NetAddress _address, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount)
	: m_type(_type), m_netAddress(_address), m_iocpCore(_core), m_sessionFactory(_factory), m_maxSessionCount(_maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{
	// TODO
}

void Service::Broadcast(SendBufferRef _sendBuffer)
{
	WRITE_LOCK;
	for (const auto& session : m_sessions)
	{
		session->Send(_sendBuffer);
	}
}

SessionRef Service::CreateSession()
{
	SessionRef session = m_sessionFactory();
	session->SetService(shared_from_this());

	if (m_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef _session)
{
	WRITE_LOCK;
	m_sessionCount++;
	m_sessions.insert(_session);
}

void Service::ReleaseSession(SessionRef _session)
{
	WRITE_LOCK;

	ASSERT_CRASH(m_sessions.erase(_session) != 0);
	m_sessionCount--;
}

/*-----------------
	ClientService
------------------*/

ClientService::ClientService(NetAddress _targetAddress, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount)
	: Service(ServiceType::Client, _targetAddress, _core, _factory, _maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

ServerService::ServerService(NetAddress _address, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount)
	: Service(ServiceType::Server, _address, _core, _factory, _maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	m_listener = MakeShared<Listener>();
	if (m_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (m_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	// TODO

	Service::CloseService();
}
