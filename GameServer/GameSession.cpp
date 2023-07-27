#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* _buffer, int32 _len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, _buffer, _len);
}

void GameSession::OnSend(int32 _len)
{
	//cout << "OnSend Len = " << _len << endl;
}