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
	// 사이즈 200이다(중요)
	WCHAR buffer[100];
	// sizeof(buffer) / sizeof(WCHAR)을 매번 쓰기 위험해 매크로(len32)로 바꿈
	::InetNtopW(AF_INET, &m_sockAddr.sin_addr, buffer, len32(buffer));
	return wstring(buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* _ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, _ip, &address);

	return address;
}