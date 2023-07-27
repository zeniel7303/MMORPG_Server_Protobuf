#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
private:
	map<uint64, PlayerRef> m_players;

	// 직접 접근 불가능하도록 private
public:
	// 싱글쓰레드 환경으로 구현
	void Enter(PlayerRef _player);
	void Leave(PlayerRef _player);
	void Broadcast(SendBufferRef _sendBuffer);

public:
	// 멀티쓰레드 환경에서는 job으로 접근시켜야함
	virtual void FlushJob() override;
};

extern shared_ptr<Room> GRoom;