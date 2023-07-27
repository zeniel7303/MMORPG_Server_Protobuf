/*
#include "pch.h"
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "ThreadManager.h"

#include "..//ServerCore/RefCounting.h"
#include "Memory.h"
#include "..//ServerCore/Allocator.h"
#include "TypeCast.h"

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
public:
	DECLARE_TL

	Player()
	{
		INIT_TL(Player);
	}
	virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight() { INIT_TL(Knight); }
};

class Mage : public Player
{
public:
	Mage() { INIT_TL(Mage); }
};

class Archer : public Player
{
public:
	Archer() { INIT_TL(Archer); }
};

class Dog
{

};

int main()
{
	//TypeList<Mage, TypeList<Knight, Archer>>::Head value1;
	//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head value2;
	//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail value3;
	//
	//int32 len1 = Length<TypeList<Mage, Knight>>::value;
	//int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;
	//
	//TypeAt<TL, 0>::Result value4;
	//TypeAt<TL, 1>::Result value5;
	//TypeAt<TL, 2>::Result value6;
	//
	//int32 index1 = IndexOf<TL, Mage>::value;
	//int32 index2 = IndexOf<TL, Knight>::value;
	//int32 index3 = IndexOf<TL, Archer>::value;
	//int32 index4 = IndexOf<TL, Dog>::value;
	//
	//bool canConvert1 = Conversion<Player, Knight>::exists;
	//bool canConvert2 = Conversion<Knight, Player>::exists;
	//bool canConvert3 = Conversion<Knight, Dog>::exists;

	{
		//Player* player = new Knight();
		
		//bool canCast = CanCast<Knight*>(player);
		//Knight* knight = TypeCast<Knight*>(player);
		
		//delete player;
	}

	{
		shared_ptr<Knight> knight = MakeShared<Knight>();
		shared_ptr<Player> player1 = TypeCast<Player>(knight);
		bool canCast1 = CanCast<Player>(knight);

		shared_ptr<Player> player2 = MakeShared<Knight>();
		shared_ptr<Archer> archer = TypeCast<Archer>(player2);
		bool canCast2 = CanCast<Mage>(player2);
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{

				}
			});
	}

	GThreadManager->Join();
}
*/