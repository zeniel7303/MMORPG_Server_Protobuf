#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

char sendData[] = "Hello World";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession()" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;

		Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnRecvPacket(BYTE* _buffer, int32 _len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);

		// TODO : packetId 대역 체크
		ServerPacketHandler::HandlePacket(session, _buffer, _len);
	}

	virtual void OnSend(int32 _len) override
	{
		//cout << "OnSend Len = " << _len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,	// TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					// 자동으로 Listener의 Dispatch도 들어간다.(가상함수이므로)
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(u8"Hello World !");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);

	while (true)
	{
		service->Broadcast(sendBuffer);
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();
}