#pragma once
#include <functional>

/*----------------------------
			  Job
------------------------------*/

using CallbackType = std::function<void()>;

// Functor + Lambda
class Job
{
private:
	CallbackType m_callback;

public:
	Job(CallbackType&& _callback) : m_callback(std::move(_callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	// shared_ptr로 생명 유지는 쉽지만 Cycle일어나는 것 주의(Memory leak)
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