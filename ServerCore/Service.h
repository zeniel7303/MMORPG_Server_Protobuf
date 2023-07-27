#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

/*-------------
	Service
--------------*/

using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
protected:
	USE_LOCK;
	ServiceType			m_type;
	NetAddress			m_netAddress = {};
	IocpCoreRef			m_iocpCore;

	Set<SessionRef>		m_sessions;
	int32				m_sessionCount = 0;
	int32				m_maxSessionCount = 0;
	SessionFactory		m_sessionFactory;

public:
	Service(ServiceType _type, NetAddress _address, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return m_sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory _func) { m_sessionFactory = _func; }

	void				Broadcast(SendBufferRef _sendBuffer);
	SessionRef			CreateSession();
	void				AddSession(SessionRef _session);
	void				ReleaseSession(SessionRef _session);
	int32				GetCurrentSessionCount() { return m_sessionCount; }
	int32				GetMaxSessionCount() { return m_maxSessionCount; }

public:
	ServiceType			GetServiceType() { return m_type; }
	NetAddress			GetNetAddress() { return m_netAddress; }
	IocpCoreRef&		GetIocpCore() { return m_iocpCore; }
};

/*-----------------
	ClientService
------------------*/

class ClientService : public Service
{
public:
	ClientService(NetAddress _targetAddress, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};


/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
private:
	ListenerRef		m_listener = nullptr;

public:
	ServerService(NetAddress _targetAddress, IocpCoreRef _core, SessionFactory _factory, int32 _maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;
};