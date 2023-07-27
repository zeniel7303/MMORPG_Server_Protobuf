#pragma once

class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef _session);
	void Remove(GameSessionRef _session);
	void Broadcast(SendBufferRef _sendBuffer);

private:
	USE_LOCK;
	Set<GameSessionRef> m_sessions;
};

extern GameSessionManager GSessionManager;
