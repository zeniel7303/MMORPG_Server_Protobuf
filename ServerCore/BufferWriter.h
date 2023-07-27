#pragma once

/*----------------------------
		  BufferWriter
------------------------------*/

class BufferWriter
{
private:
	BYTE*			m_buffer = nullptr;
	uint32			m_size = 0;
	uint32			m_pos = 0;

public:
	BufferWriter();
	BufferWriter(BYTE* _buffer, uint32 _size, uint32 _pos = 0);
	~BufferWriter();

	BYTE*			Buffer() { return m_buffer; }
	uint32			Size() { return m_size; }
	uint32			WriteSize() { return m_pos; }
	uint32			FreeSize() { return m_size - m_pos; }

	template<typename T>
	bool			Write(T* _src) { return Write(_src, sizeof(T)); }
	bool			Write(void* _src, uint32 _len);

	template<typename T>
	T*				Reserve(uint16 _count = 1);

	// 보편 참조
	// https://lakanto.tistory.com/46
	// 원래는 && 붙으면 오른값 참조이긴하다만 template이 붙으면 보편 참조가 된다.
	template<typename T>
	BufferWriter&	operator<<(T&& _src);
};

template<typename T>
T* BufferWriter::Reserve(uint16 _count)
{
	if (FreeSize() < (sizeof(T) * _count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&m_buffer[m_pos]);
	m_pos += (sizeof(T) * _count);
	return ret;
}

// 보편 참조
// 왼값이면 const T&
// 오른값이면 T&&
// 로 바뀜
template<typename T>
BufferWriter& BufferWriter::operator<<(T&& _src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&m_buffer[m_pos]) = std::forward<DataType>(_src);
	m_pos += sizeof(T);
	return *this;
}