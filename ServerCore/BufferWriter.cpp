#include "pch.h"
#include "BufferWriter.h"

/*----------------------------
		  BufferWriter
------------------------------*/

BufferWriter::BufferWriter()
{

}

BufferWriter::BufferWriter(BYTE* _buffer, uint32 _size, uint32 _pos)
	: m_buffer(_buffer), m_size(_size), m_pos(_pos)
{

}

BufferWriter::~BufferWriter()
{

}

bool BufferWriter::Write(void* _src, uint32 _len)
{
	if (FreeSize() < _len)
		return false;

	::memcpy(&m_buffer[m_pos], _src, _len);
	m_pos += _len;
	return true;
}