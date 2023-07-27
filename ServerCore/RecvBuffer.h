#pragma once

/*----------------------------
		  RecvBuffer
------------------------------*/


class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

private:
	int32			m_capacity = 0;
	int32			m_bufferSize = 0;
	int32			m_readPos = 0;
	int32			m_writePos = 0;
	Vector<BYTE>	m_buffer;

public:
	RecvBuffer(int32 _bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int32 _numOfBytes);
	bool			OnWrite(int32 _numOfBytes);

	BYTE*			ReadPos() { return &m_buffer[m_readPos]; }
	BYTE*			WritePos() { return &m_buffer[m_writePos]; }
	int32			DataSize() { return m_writePos - m_readPos; }
	int32			FreeSize() { return m_capacity - m_writePos; }
};

