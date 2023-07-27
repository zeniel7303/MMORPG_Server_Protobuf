#pragma once
#include "Types.h"

/*----------------------------
          RW SpinLock
------------------------------*/

/*----------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
------------------------------*/

// Write는 연속적으로 가능하다.
// 그리고 Write를 잡은 상태에서 Read도 가능하다
// 하지만 반대는 불가능하다.
// W -> W (O)
// W -> R (O)
// R -> W (X)
class Lock
{
    enum : uint32
    {
        ACQUIRE_TIMEOUT_TICK = 10000,
        MAX_SPIN_COUNT = 5000,
        WRITE_THREAD_MASK = 0xFFFF'0000,
        READ_COUNT_MASK = 0x0000'FFFF,
        EMPTY_FLAG = 0x0000'0000
    };

private:
    Atomic<uint32>  m_lockFlag = EMPTY_FLAG;
    uint16          m_writeCount = 0;

public:
    void WriteLock(const char* _name);
    void WriteUnlock(const char* _name);
    void ReadLock(const char* _name);
    void ReadUnlock(const char* _name);
};

/*----------------------------
          LockGuards
------------------------------*/

// RAII
class ReadLockGuard
{
private:
    Lock& m_lock;
    const char* m_name;

public:
	ReadLockGuard(Lock& _lock, const char* _name) : m_lock(_lock), m_name(_name)
    { 
        m_lock.ReadLock(m_name); 
    }

	~ReadLockGuard() { m_lock.ReadUnlock(m_name); }
};

// RAII
class WriteLockGuard
{
private:
    Lock& m_lock;
    const char* m_name;

public:
	WriteLockGuard(Lock& _lock, const char* _name) : m_lock(_lock), m_name(_name)
    { 
        m_lock.WriteLock(m_name);
    }

	~WriteLockGuard() { m_lock.WriteUnlock(m_name); }
};