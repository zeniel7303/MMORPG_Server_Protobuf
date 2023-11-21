#pragma once
class Room;

using RoomRef = shared_ptr<Room>;

class RoomManager
{
public:
	static RoomManager* getSingleton()
	{
		static RoomManager singleton;

		return &singleton;
	}

private:
	USE_LOCK;
	Set<RoomRef> m_rooms;

public:
	void Add(RoomRef _room);
	void Remove(RoomRef _room);
	RoomRef GetRoom();
};

