#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Enter(PlayerRef _player)
{
	m_players[_player->playerId] = _player;
}

void Room::Leave(PlayerRef _player)
{
	m_players.erase(_player->playerId);
}

void Room::Broadcast(SendBufferRef _sendBuffer)
{
	for (auto& p : m_players)
	{
		p.second->ownerSession->Send(_sendBuffer);
	}
}

void Room::FlushJob()
{
	while (true)
	{
		JobRef job = m_jobQueue.Pop();
		if (job == nullptr)
			break;

		job->Execute();
	}
}