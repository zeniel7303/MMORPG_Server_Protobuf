#include "pch.h"
#include "RecvBuffer.h"

/*----------------------------
		  RecvBuffer
------------------------------*/

RecvBuffer::RecvBuffer(int32 _bufferSize) : m_bufferSize(_bufferSize)
{
	m_capacity = _bufferSize * BUFFER_COUNT;
	m_buffer.resize(m_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// 딱 마침 읽기+쓰기 커서가 동일한 위치라면, 둘 다 리셋.
		m_readPos = m_writePos = 0;
	}
	else
	{
		// [ 이렇게 하는 이유 : 최대한 데이터를 복사하는 상황을 안만들기 위해 버퍼 크기를 많이 잡아두고
		// readPos와 writePos가 같아 지는 순간이 생길 확률을 높여 데이터를 복사하는 상황을 줄여
		// 최적화 하기 위함이다. ]
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (FreeSize() < m_bufferSize)
		{
			::memcpy(&m_buffer[0], &m_buffer[m_readPos], dataSize);
			m_readPos = 0;
			m_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 _numOfBytes)
{
	if (_numOfBytes > DataSize())
		return false;

	m_readPos += _numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 _numOfBytes)
{
	if (_numOfBytes > FreeSize())
		return false;

	m_writePos += _numOfBytes;
	return true;
}