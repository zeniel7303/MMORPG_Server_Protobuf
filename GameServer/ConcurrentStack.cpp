#include "pch.h"
#include "ConcurrentStack.h"

/*----------------------------
		   1차 시도
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->next = nullptr;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	_entry->next = _header->next;
	_header->next = _entry;
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListEntry* first = _header->next;

	if (first != nullptr)
		_header->next = first->next;

	return first;
}*/

/*----------------------------
		   2차 시도
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->next = nullptr;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	_entry->next = _header->next;
															     // desired, expected
	while (::InterlockedCompareExchange64(
		(int64*)&_header->next, (int64)_entry, (int64)_entry->next) == 0)
	{

	}
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListEntry* expected = _header->next;

	// ABA 문제
	while (expected && ::InterlockedCompareExchange64(
		(int64*)&_header->next, (int64)expected->next, (int64)expected) == 0)
	{

	}

	return expected;
}*/

/*----------------------------
		   3차 시도
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->alignment = 0;
	_header->region = 0;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	SListHeader expected = {};
	SListHeader desired = {};

	// 16바이트 정렬
	// 60비트(next 크기)로 주소를 표현
	desired.HeaderX64.next = (((uint64)_entry) >> 4);

	while (true)
	{
		expected = *_header;

		// 이 사이에 변경될 수 있다.

		// 4비트를 날려 관리했던 것을 복원
		_entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		// 카운팅
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

			//									대상
		if (::InterlockedCompareExchange128((int64*)_header, 
			// exchange 값 2개					예측한 값
			desired.region, desired.alignment, (int64*)&expected) == 1)
			// 성공
			break;
	}
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *_header;
		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		if (entry == nullptr) break;

		// 이곳도 Use_After_Free 문제 있을 수 있다.
		// 64비트 주소를 60비트에 낑겨넣기 위해 4를 땡긴다.
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;

		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)_header,
			desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}*/

// GameServer.cpp에서 가져옴(예제 및 직접 만든것과 거의 차이가 없음을 보여줌)
/*
DECLSPEC_ALIGN(16)
class Data
{
public:
	// 상속받거나 맴버 변수로 활용하면된다.
	SLIST_ENTRY m_entry; //SListEntry m_entry;

	int64 m_rand = rand() % 1000;
};

SLIST_HEADER* GHeader; //SListHeader* GHeader;

int main()
{
	GHeader = new SLIST_HEADER(); //GHeader = new SListHeader();
	// 16바이트 정렬인가 체크
	ASSERT_CRASH((uint64)GHeader % 16 == 0);
	::InitializeSListHead(GHeader); //InitializeHead(GHeader);

	for (int32 i = 0; i < 3; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* data = new Data();
					ASSERT_CRASH((uint64)data % 16 == 0);

					::InterlockedPushEntrySList(GHeader, (PSLIST_ENTRY)data);
					//PushEntrySList(GHeader, (SListEntry*)data);
					this_thread::sleep_for(10ms);
				}
			});
	}

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* popData = nullptr;
					popData = (Data*)::InterlockedPopEntrySList(GHeader);
					//popData = (Data*)PopEntrySList(GHeader);

					if (popData)
					{
						cout << popData->m_rand << endl;
					}
					else
					{
						cout << "NONE" << endl;
					}

					this_thread::sleep_for(5ms);
				}
			});
	}

	GThreadManager->Join();
}
*/