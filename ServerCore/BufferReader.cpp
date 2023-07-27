#include "pch.h"
#include "BufferReader.h"

/*----------------------------
		  BufferReader
------------------------------*/

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* _buffer, uint32 _size, uint32 _pos)
	: m_buffer(_buffer), m_size(_size), m_pos(_pos)
{

}

BufferReader::~BufferReader()
{

}

bool BufferReader::Peek(void* _dest, uint32 _len)
{
	if (FreeSize() < _len)
		return false;

	::memcpy(_dest, &m_buffer[m_pos], _len);
	return true;
}

bool BufferReader::Read(void* _dest, uint32 _len)
{
	if (Peek(_dest, _len) == false)
		return false;

	m_pos += _len;
	return true;
}
