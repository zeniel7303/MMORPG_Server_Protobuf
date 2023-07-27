#pragma once
/*
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;

	ready.store(true, memory_order::memory_order_seq_cst); // = ready.store(true);
	//ready.store(true, memory_order::memory_order_release);
	//std::atomic_thread_fence(memory_order::memory_order_release);
	// ---------------- 절취선 -----------------
}

void Consumer()
{
	// ---------------- 절취선 -----------------
	//std::atomic_thread_fence(memory_order::memory_order_acquire);
	//while (ready.load(memory_order::memory_order_acquire) == false);
	while (ready.load(memory_order::memory_order_seq_cst) == false); // = ready.load();

	cout << value << endl;
}

void main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	// Memory Model (정책)
	// 1) Sequentially Consistent (seq_cst)
	// 2) Acquire-Release (consume, acquire, release, acq_rel[acquire + release]) *consume은 이슈가 있음
	// 3) Relaxed (relaxed)

	// 1) seq_cst (가장 엄격 = 컴파일러 최적화 여지 적음 = 직관적)
	// 가시성 문제 바로 해결, 코드 재배치 바로 해결

	// 2) acquire-Release
	// 딱 중간
	// release 명령 이전의 메모리 명령들이 해당 명령 이후로 재배치 되는 것을 금지
	// 그리고 acquire로 같은 변수로 읽는 쓰레드가 있다면
	// release 이전의 명령들이 -> acquire 하는 순간에 관찰 가능하다. (가시성 보장)

	// 3) relaxed (자유롭다 = 컴파일러 최적화 여지 많음 = 직관적이지 않음)
	// 너무나도 편안하고 자유롭다.
	// 코드 재배치도 멋대로 가능, 가시성 해결 불가능
	// 가장 기본 조건 (동일 객체에 대한 동일 관전 순서)만 보장

	// 인텔, AMD의 경우 애당초 순차적 일관성을 보장을 해서,
	// seq_cst를 써도 별다른 부하가 없다.
	// ARM의 경우 꽤 차이가 있다고 한다.
	// NC에서 면접으로 나온 경우가 있다.
}
*/