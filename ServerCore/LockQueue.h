#pragma once

/*----------------------------
			LockQueue
------------------------------*/

template <typename T>
class LockQueue
{
private:
	USE_LOCK;
	Queue<T> m_items;

public:
	void Push(T _item)
	{
		WRITE_LOCK;
		m_items.push(_item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (m_items.empty())
			return T();

		T ret = m_items.front();
		m_items.pop();
		return ret;
	}

	void PopAll(OUT Vector<T>& _items)
	{
		WRITE_LOCK;
		while (T item = Pop())
			_items.push_back(item);
	}

	void Clear()
	{
		WRITE_LOCK;
		m_items = Queue<T>();
	}
};
