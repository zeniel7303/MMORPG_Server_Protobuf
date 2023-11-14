#include "pch.h"
#include "SendBuffer.h"

/*----------------------------
		  SendBuffer
------------------------------*/

SendBuffer::SendBuffer(SendBufferChunkRef _owner, BYTE* _buffer, uint32 _allocSize)
	: m_owner(_owner), m_buffer(_buffer), m_allocSize(_allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 _writeSize)
{
	ASSERT_CRASH(m_allocSize >= _writeSize);
	m_writeSize = _writeSize;
	m_owner->Close(_writeSize);
}

/*----------------------------
		SendBufferChunk
------------------------------*/

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	m_open = false;
	m_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 _allocSize)
{
	ASSERT_CRASH(_allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(m_open == false);

	if (_allocSize > FreeSize())
		return nullptr;

	m_open = true;
	//												내 자신			버퍼			
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), _allocSize);
}

void SendBufferChunk::Close(uint32 _writeSize)
{
	ASSERT_CRASH(m_open == true);
	m_open = false;
	m_usedSize += _writeSize;
}

/*----------------------------
		SendBufferManager
------------------------------*/

SendBufferRef SendBufferManager::Open(uint32 _size)
{
	// TLS 이므로 남들과 어지간하면 경합하지 않는다.
	// 교체를 실행하는 Pop만 락 사용
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}		

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 여유공간을 체크해 다 썼으면 버리고 새거로 교체
	// (버려진 LSendBufferChunk는 RefCount가 0이 되어 PushGlobal을 호출한다.)
	if (LSendBufferChunk->FreeSize() < _size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	//cout << "FREE : " << LSendBufferChunk->FreeSize() << endl;

	return LSendBufferChunk->Open(_size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	//cout << "Pop SENDBUFFERCHUNK" << endl;

	{
		WRITE_LOCK;
		if (m_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = m_sendBufferChunks.back();
			m_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	// 여유분이 없으므로 새로 생성
	// 무한이 재사용할 것이므로 deleter 자리에 delete로 메모리를 날리는게 아닌 PushGlobal을 넣는다.
	// PushGlobal은 맴버함수로 전달할 수 없으므로 static 함수로 구현
	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef _buffer)
{
	WRITE_LOCK;
	m_sendBufferChunks.push_back(_buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* _buffer)
{
#ifdef _DEBUG
	cout << "PushGlobal SENDBUFFERCHUNK" << endl;
#endif

	GSendBufferManager->Push(SendBufferChunkRef(_buffer, PushGlobal));
}