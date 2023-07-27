#include "pch.h"
#include "NetAddress.h"

/*----------------------------
		  NetAddress
------------------------------*/

NetAddress::NetAddress(SOCKADDR_IN _sockAddr) : m_sockAddr(_sockAddr)
{
}

NetAddress::NetAddress(wstring _ip, uint16 _port)
{
	::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
	m_sockAddr.sin_family = AF_INET;
	m_sockAddr.sin_addr = Ip2Address(_ip.c_str());
	m_sockAddr.sin_port = ::htons(_port);
}

wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &m_sockAddr.sin_addr, buffer, len32(buffer));
	return wstring(buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* _ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, _ip, &address);

	return address;
}