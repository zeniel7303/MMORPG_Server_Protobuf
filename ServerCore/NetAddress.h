#pragma once

/*----------------------------
		  NetAddress
------------------------------*/

class NetAddress
{
private:
	SOCKADDR_IN		m_sockAddr = {};

public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN _sockAddr);
	NetAddress(wstring _ip, uint16 _port);

	SOCKADDR_IN&	GetSockAddr() { return m_sockAddr; }
	wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(m_sockAddr.sin_port); }

public:
	static IN_ADDR	Ip2Address(const WCHAR* _ip);
};

