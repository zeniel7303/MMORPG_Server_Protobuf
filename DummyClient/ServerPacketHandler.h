#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& _session, BYTE* _buffer, int32 _len);
bool Handle_S_LOGIN(PacketSessionRef& _session, Protocol::S_LOGIN& _pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& _session, Protocol::S_ENTER_GAME& _pkt);
bool Handle_S_CHAT(PacketSessionRef& _session, Protocol::S_CHAT& _pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, _session, _buffer, _len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, _session, _buffer, _len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, _session, _buffer, _len); };
	}

	static bool HandlePacket(PacketSessionRef& _session, BYTE* _buffer, int32 _len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
		return GPacketHandler[header->id](_session, _buffer, _len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& _pkt) { return MakeSendBuffer(_pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& _pkt) { return MakeSendBuffer(_pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& _pkt) { return MakeSendBuffer(_pkt, PKT_C_CHAT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc _func, PacketSessionRef& _session, BYTE* _buffer, int32 _len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(_buffer + sizeof(PacketHeader), _len - sizeof(PacketHeader)) == false)
			return false;

		return _func(_session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& _pkt, uint16 _pktId)
	{
		const uint16 dataSize = static_cast<uint16>(_pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = _pktId;
		ASSERT_CRASH(_pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};