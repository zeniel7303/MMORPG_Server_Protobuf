#include "pch.h"

//#include <vector>
//#include <thread>
//
// 소수 구하기
//bool IsPrime(int _number)
//{
//	if (_number <= 1)
//		return false;
//	if (_number == 2 || _number == 3)
//		return true;
//
//	for (int i = 2; i < _number; i++)
//	{
//		if ((_number % i) == 0)
//			return false;
//	}
//
//	return true;
//}
//
//int CountPrime(int _start, int _end)
//{
//	int count = 0;
//	for (int number = _start; number <= _end; number++)
//	{
//		if (IsPrime(number)) count++;
//	}
//
//	return count;
//}
//
//int main()
//{
//	const int MAX_NUMBER = 1000000;
//	// 1 ~ MAX_NUMBER까지의 소수 갯수 구하기
//
//	// 1000 = 168
//	// 10000 = 1229
//	// 1000000 = 78498
//
//	vector<thread> threads;
//
//	int coreCount = thread::hardware_concurrency();
//	int jobCount = (MAX_NUMBER / coreCount) + 1;
//
//	atomic<int> primeCount = 0;
//
//	for (int i = 0; i < coreCount; i++)
//	{
//		int start = (i * jobCount) + 1;
//		int end = min(MAX_NUMBER, ((i + 1) * jobCount));
//
//		threads.push_back(thread([start, end, &primeCount]()
//			{
//				primeCount += CountPrime(start, end);
//			}));
//	}
//
//	for (thread& t : threads)
//		t.join();
//
//	cout << primeCount << endl;
//}