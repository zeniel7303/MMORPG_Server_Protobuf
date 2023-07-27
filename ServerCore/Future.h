#pragma once
/*
#include <thread>
#include <future>

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100000; i++)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

void main()
{
	// 동기(Synchronous) 실행
	//int64 sum = Calculate();
	//cout << sum << endl;

	// std::future
	// 게임 서버 시작시 데이터 시트 로딩에서 사용했다고 한다.
	// 멀티쓰레드로 해도 좋지만 어차피 데이터 로딩때만 쓰므로 이 방식이 유용해보인다.
	{
		// 1) deferred : lazy evaluation 지연해서 실행하세요.
		// 2) async : 별도의 쓰레드를 만들어서 실행하세요.
		// 3) deferred | async : 둘 중 알아서 골라주세요.

		// 언젠가 미래에 결과물을 뱉어줄거야!
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// TODO
		std::future_status status = future.wait_for(1ms);
		if (status == future_status::ready)
		{
			// 일감이 완료되었으니 처리
		}

		int64 sum = future.get(); // = future.wait(); 결과물이 이제서야 필요하다!

		// 맴버 함수 이용법
		{
			class Knight
			{
			public:
				int64 GetHP() { return 100; }
			};

			Knight knight;
			std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight); //knight.GetHp();
		}
	}

	//std::promise
	{
		// 미래에(std::future)에 결과물을 반화해줄꺼라 약속(std::promise) 해줘
		std::promise<string> promise;
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise));

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	// async future는 간접적인 쓰레드를 만들어 결과물을 받아내는 것과 다르게 
	// 이미 존재하는 쓰레드에 일감이란 개념을 만들어 떠넘겨주고 결과물을 future로 받는다.
	{
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		thread t(TaskWorker, std::move(task));

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}

	// 결론)
	// mutex, condition_variable까지 가지 않고 단순한 애들을 처리할 수 있는
	// 특히나, 한 번 발생하는 이벤트에 유용하다!
	// 닭잡는데 소잡는 칼을 쓸 필요 없다!
	// 1) async
	// 원하는 함수 하나를 비동기적으로 실행
	// 2) promise (거의 사용X)
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packaged_task (거의 사용X)
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌
}
*/