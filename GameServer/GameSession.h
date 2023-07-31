#pragma once
#include "Session.h"

class GameSession : public PacketSession
{
public:
	Vector<PlayerRef>		m_players;

	PlayerRef				m_currentPlayer;
	weak_ptr<class Room>	m_room;

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