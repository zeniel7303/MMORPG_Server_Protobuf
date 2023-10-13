#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_TEST = 1000,
	PKT_C_MOVE = 1001,
	PKT_S_TEST = 1002,
	PKT_S_LOGIN = 1003,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& _session, BYTE* _buffer, int32 _len);
bool Handle_C_TEST(PacketSessionRef& _session, Protocol::C_TEST& _pkt);
bool Handle_C_MOVE(PacketSessionRef& _session, Protocol::C_MOVE& _pkt);

class TestPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_TEST] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, _session, _buffer, _len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, _session, _buffer, _len); };
	}

	static bool HandlePacket(PacketSessionRef& _session, BYTE* _buffer, int32 _len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
		return GPacketHandler[header->id](_session, _buffer, _len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& _pkt) { return MakeSendBuffer(_pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& _pkt) { return MakeSendBuffer(_pkt, PKT_S_LOGIN); }

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