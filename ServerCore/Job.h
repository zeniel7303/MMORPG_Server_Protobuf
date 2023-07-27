#pragma once
#include <functional>

/*----------------------------
			  Job
------------------------------*/

using CallbackType = std::function<void()>;

class Job
{
private:
	CallbackType m_callback;

public:
	Job(CallbackType&& _callback) : m_callback(std::move(_callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> _owner, Ret(T::* _memFunc)(Args...), Args&&... _args)
	{
		m_callback = [_owner, _memFunc, _args...]()
		{
			(_owner.get()->*_memFunc)(_args...);
		};
	}

	void Execute()
	{
		m_callback();
	}
};