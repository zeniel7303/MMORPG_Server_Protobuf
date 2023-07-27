#pragma once

class SendBufferChunk;

/*----------------------------
		  SendBuffer
------------------------------*/

class SendBuffer
{
private:
	BYTE*				m_buffer;
	uint32				m_allocSize = 0;
	uint32				m_writeSize = 0;
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

	void				Reset();
	SendBufferRef		Open(uint32 _allocSize);
	void				Close(uint32 _writeSize);

	bool				IsOpen() { return m_open; }
	BYTE*				Buffer() { return &m_buffer[m_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(m_buffer.size()) - m_usedSize; }
};

/*----------------------------
		SendBufferManager
------------------------------*/

class SendBufferManager
{
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> m_sendBufferChunks;

public:
	SendBufferRef		Open(uint32 _size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef _buffer);

	static void			PushGlobal(SendBufferChunk* _buffer);
};
