#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager GSessionManager;

void GameSessionManager::Add(GameSessionRef _session)
{
	WRITE_LOCK;
	m_sessions.insert(_session);
}

void GameSessionManager::Remove(GameSessionRef _session)
{
	WRITE_LOCK;
	m_sessions.erase(_session);
}

void GameSessionManager::Broadcast(SendBufferRef _sendBuffer)
{
	WRITE_LOCK;
	for (GameSessionRef session : m_sessions)
	{
		session->Send(_sendBuffer);
	}
}