#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

/*----------------------------
		  IocpCore
------------------------------*/

IocpCore::IocpCore()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(m_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef _iocpObject)
{
	// return ::CreateIoCompletionPort(_iocpObject->GetHandle(), m_iocpHandle, reinterpret_cast<ULONG_PTR>(_iocpObject), 0);

	// CICS에서 Key값으로 등록한 iocpObject와, GQCS에서 복원해서 작업하고 있었지만 해당 작업을 하고 있을때 iocpObject가 살아있다는 보장이 없다.
	// 만약 죽어있다면 Crash가 날 것이다. 그러므로 둘 다 살아있음을 보장해줘야한다.
	// 해결책
	// 1. RefCounting -> 문제점으로 외부에서 Session을 shared_ptr등으로 이중으로 RefCounting을 시작하면 모순이 생긴다. (외부에서 Ref가 0이 되어서 삭제하려하면 내부와는 관계없이 삭제되어 문제가 됨)
	//					 물론 멤버 변수로 자기 자신의 shared_ptr을 물려 사용하면 되긴하지만 깔끔하지 않다.
	// 2. key값을 사용하지 않고 모든 것들을 IocpEvent로 물려준다.(IocpEvent에서 자기 자신을 실질적으로 걸어준 IocpObject를 기억한다.) <- 해당 방법으로 진행

	// 소켓, session 주소 뿐만 아니라 queue를 사용하듯 원하는 일감을 넣을 수도 있다. -> IocpObject로 관리
	return ::CreateIoCompletionPort(_iocpObject->GetHandle(), m_iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 _timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	/*IocpObject* iocpObject = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, OUT & numOfBytes,
		OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), _timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}*/
	if (::GetQueuedCompletionStatus(m_iocpHandle, OUT &numOfBytes, OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), _timeoutMs))
	{
		// 성공
		IocpObjectRef iocpObject = iocpEvent->m_owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		// 실패
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : 로그 찍기
			IocpObjectRef iocpObject = iocpEvent->m_owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
