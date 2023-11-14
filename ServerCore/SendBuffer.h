#pragma once

class SendBufferChunk;

/*----------------------------
		  SendBuffer
------------------------------*/

// 최종적으로 SendBufferChunk를 잘라서 사용하는 것이 이 SendBuffer다.(SendBufferChunk에서 잘라온 것을 SendBuffer로 관리)
class SendBuffer
{
private:
	BYTE*				m_buffer;
	// 할당된 사이즈
	uint32				m_allocSize = 0;
	// 실제로 사용하는 데이터 사이즈
	uint32				m_writeSize = 0;
	// 누구라도 SendBufferChunk의 일부 영역을 사용 중이면 SendBufferChunk를 삭제하면 안되므로
	// RefCounting을 (유지)하기위해 SendBuffer가 자신을 포함하고있는 SendBufferChunk를 알아야한다.
	SendBufferChunkRef	m_owner;

public:
	SendBuffer(SendBufferChunkRef _owner, BYTE* _buffer, uint32 _allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return m_buffer; }
	uint32		AllocSize() { return m_allocSize; }
	uint32		WriteSize() { return m_writeSize; }
	void		Close(uint32 _writeSize);
};

/*----------------------------
		SendBufferChunk
------------------------------*/

// SendBufferManager에서 TLS를 사용해 SendBufferChunk를 꺼내 사용하므로 멀티쓰레드 환경이 아닌 싱글쓰레드 환경으로 구현하면된다.(Lock 필요X)
// enable_shared_from_this 쓸 때 public을 누락하면 에러가 난다.(자신의 weak_ptr을 채워주는 부분이 실행되지 않기 때문)
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		m_buffer = {};
	bool									m_open = false;
	uint32									m_usedSize = 0;

public:
	SendBufferChunk();
	~SendBufferChunk();

	// 초기화
	void				Reset();
	SendBufferRef		Open(uint32 _allocSize);
	void				Close(uint32 _writeSize);

	bool				IsOpen() { return m_open; }
	// m_usedSize까지 사용했으니 그 이후부터 사용하도록 해당 버퍼 포인터 반환
	BYTE*				Buffer() { return &m_buffer[m_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(m_buffer.size()) - m_usedSize; }
};

/*----------------------------
		SendBufferManager
------------------------------*/

// 쓰레드별로 SendBuffer를 둬서(TLS 활용) Lock을 대부분의 상황에서 걸지 않도록 구현
class SendBufferManager
{
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> m_sendBufferChunks;

public:
	// 실질적으로 컨탠츠에서 사용할 함수
	// 미리 만들어둔 큰 버퍼에서 사용할 크기만큼 쪼개서 사용하도록 열어주는 함수
	SendBufferRef		Open(uint32 _size);

private:
	// 꺼내쓰기
	SendBufferChunkRef	Pop();
	// 반납
	void				Push(SendBufferChunkRef _buffer);

	static void			PushGlobal(SendBufferChunk* _buffer);
};
