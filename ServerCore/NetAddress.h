#pragma once

/*----------------------------
		  NetAddress
------------------------------*/

// 클라이언트의 ip 주소 추출이 필요할 수도 있음
// -> 함수를 매번 부르기보단 모든 주소를 해당 클래스로 래핑을 해 사용해서 편하게 설정하고
// 원하는 정보를 추출할 수 있게 만든 클래스
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