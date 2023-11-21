#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

void RoomManager::Add(RoomRef _room)
{
	WRITE_LOCK;
	m_rooms.insert(_room);
}

void RoomManager::Remove(RoomRef _room)
{
	WRITE_LOCK;
	m_rooms.erase(_room);
}

RoomRef RoomManager::GetRoom()
{
	return *m_rooms.begin();
}