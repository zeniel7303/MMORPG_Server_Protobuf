#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& _session, BYTE* _buffer, int32 _len);

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(PacketSessionRef& _session, Protocol::{{pkt.name}}& _pkt);
{%- endfor %}

class {{output}}
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](PacketSessionRef& _session, BYTE* _buffer, int32 _len) { return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, _session, _buffer, _len); };
{%- endfor %}
	}

	static bool HandlePacket(PacketSessionRef& _session, BYTE* _buffer, int32 _len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
		return GPacketHandler[header->id](_session, _buffer, _len);
	}

{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& _pkt) { return MakeSendBuffer(_pkt, PKT_{{pkt.name}}); }
{%- endfor %}

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
