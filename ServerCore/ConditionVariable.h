#pragma once
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>

using namespace std;

//mutex m;
//queue<int32> q;
//
//// 참고) CV는 User-Level Object (커널 오브젝트가 아님)
//condition_variable cv;
//
//void Producer()
//{
//	while (true)
//	{
//		// 1) Lock을 잡고
//		// 2) 공유 변수 값을 수정
//		// 3) Lock을 풀고
//		// 4) 조건변수 통해 다른 쓰레드에게 통지
//
//		{
//			unique_lock<mutex> lock(m);
//			q.push(100);
//		}
//
//		cv.notify_one(); // wait 중인 쓰레드가 있으면 딱 1개만 깨운다.
//
//		//this_thread::sleep_for(10000ms);
//	}
//}
//
//void Consumer()
//{
//	while (true)
//	{
//		unique_lock<mutex> lock(m);
//		cv.wait(lock, []() {return q.empty() == false; } /*조건*/);
//		// 1) Lock을 잡고
//		// 2) 조건 확인
//		//  - 만족 O -> 빠져 나와서 이어서 코드 진행
//		//	- 만족 X -> Lock을 풀어주고 대기 상태 전환
//
//		// 그런데 notify_one을 했으면 항상 조건식을 만족하는거 아닐까?
//		// Spurious WakeUp (가짜 기상)
//		// notify_one을 할 때 lock을 잡고 있는 것이 아니기 때문
//
//		//while (q.empty() == false)
//		{
//			int32 data = q.front();
//			q.pop();
//			cout << q.size() << endl;
//		}
//	}
//}
//
//void main()
//{
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//}