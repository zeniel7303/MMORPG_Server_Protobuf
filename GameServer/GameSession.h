#pragma once
#include "Session.h"

class GameSession : public PacketSession
{
public:
	Vector<PlayerRef> m_players;

public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* _buffer, int32 _len) override;
	virtual void OnSend(int32 _len) override;
};