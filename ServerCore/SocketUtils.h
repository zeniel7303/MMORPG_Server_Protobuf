#pragma once
#include "NetAddress.h"

/*----------------------------
		  SocketUtils
------------------------------*/

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisConnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET _socket, GUID _guid, LPVOID* _fn);
	static SOCKET CreateSocket();

	static bool SetLinger(SOCKET _socket, uint16 _onoff, uint16 _linger);
	static bool SetReuseAddress(SOCKET _socket, bool _flag);
	static bool SetRecvBufferSize(SOCKET _socket, int32 _size);
	static bool SetSendBufferSize(SOCKET _socket, int32 _size);
	static bool SetTcpNoDelay(SOCKET _socket, bool _flag);
	static bool SetUpdateAcceptSocket(SOCKET _socket, SOCKET _listenSocket);

	static bool Bind(SOCKET& _socket, NetAddress _netAddr);
	static bool BindAnyAddress(SOCKET& _socket, uint16 _port);
	static bool Listen(SOCKET& _socket, int32 _backlog = SOMAXCONN);
	static void Close(SOCKET& _socket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET _socket, int32 _level, int32 _optName, T _optVal)
{
	return SOCKET_ERROR != ::setsockopt(_socket, _level, _optName,
		reinterpret_cast<char*>(&_optVal), sizeof(T));
}