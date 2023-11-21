#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
private:
	map<uint64, PlayerRef> m_players;

	// 직접 접근 불가능하도록 private
public:
	// 싱글쓰레드 환경으로 구현
	void Enter(PlayerRef _player);
	void Leave(PlayerRef _player);
	void Broadcast(SendBufferRef _sendBuffer);
};