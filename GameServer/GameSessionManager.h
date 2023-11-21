#pragma once
class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager
{
public:
	static GameSessionManager* getSingleton()
	{
		static GameSessionManager singleton;

		return &singleton;
	}

private:
	USE_LOCK;
	Set<GameSessionRef> m_sessions;

public:
	void Add(GameSessionRef _session);
	void Remove(GameSessionRef _session);
	void Broadcast(SendBufferRef _sendBuffer);
};