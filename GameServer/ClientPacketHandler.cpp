#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& _session, BYTE* _buffer, int32 _len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& _session, Protocol::C_LOGIN& _pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(_session);

	// TODO : Validation

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	// DB에서 플레이 정보를 획득하는 부분
	// GameSession에 플레이 정보를 메모리 상에 저장

	// ID 발급 (인게임 ID)
	static Atomic<uint64> idGenerator = 1;

	{
		auto player = loginPkt.add_players();
		player->set_name(u8"TempName1");
		player->set_playertype(Protocol::PLAYER_TYPE_KNIGHT);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->m_players.push_back(playerRef);
	}

	{
		auto player = loginPkt.add_players();
		player->set_name(u8"TempName2");
		player->set_playertype(Protocol::PLAYER_TYPE_MAGE);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		playerRef->name = player->name();
		playerRef->type = player->playertype();
		playerRef->ownerSession = gameSession;

		gameSession->m_players.push_back(playerRef);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	_session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& _session, Protocol::C_ENTER_GAME& _pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(_session);

	uint64 index = _pkt.playerindex();

	// TODO : Validation

	PlayerRef player = gameSession->m_players[index]; // 아직은 READ_ONLY

	GRoom->PushJob(&Room::Enter, player);

	// 추후 Jobqueue에서 해당 패킷 실행 완료 후 Send하도록 변경
	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	player->ownerSession->Send(sendBuffer);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& _session, Protocol::C_CHAT& _pkt)
{
	std::cout << _pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(_pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	GRoom->PushJob(&Room::Broadcast, sendBuffer);

	return true;
}