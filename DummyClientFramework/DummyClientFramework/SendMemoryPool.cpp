#include "stdafx.h"
#include "SendMemoryPool.h"

#include "Define.h"
#include "LogManager.h"
//---------------------------------------------------------------------------
// SendMemoryPool
//---------------------------------------------------------------------------

SendMemoryPool::SendMemoryPool()
{
	for (int i = 0; i < FRAMEWORK::MAX_COUNT_MEMORY_UNIT_OF_SEND_POOL; ++i)
		sendMemoryPool.push(/*new SendMemoryUnit()*/ std::move(new SendMemoryUnit()));

#ifdef WONSY_LOG_MANAGER
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"sendMemoryPool의 초기 할당 사이즈는" + std::to_wstring(sendMemoryPool.unsafe_size()) + L" 입니다.");
#endif
}

SendMemoryPool::~SendMemoryPool()
{
	SendMemoryUnit* pRetMemoryUnit{ nullptr };
	while (sendMemoryPool.try_pop(pRetMemoryUnit))
	{
		delete pRetMemoryUnit;
	}

#ifdef WONSY_LOG_MANAGER
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"sendMemoryPool의 소멸자가 호출되었습니다. ");
#endif
	sendMemoryPool.clear();
}

/*
	PopMemory()
		- Send에 필요한 MemoryUnit을 요청합니다.
	?0. 적절한 초기 메모리 풀 사이즈를 알기 위해, 현재 Send풀이 없을 경우, 로그를 출력합니다.
*/
SendMemoryUnit* SendMemoryPool::PopMemory()
{
	SendMemoryUnit* pRetMemoryUnit{ nullptr };

	/*while*/ if (!sendMemoryPool.try_pop(pRetMemoryUnit))
	{
		//assert(retMemoryUnit != nullptr, "retMemoryUnit의 try_pop이 nullptr을 반환했습니다.");
		
		for (int i = 0; i < FRAMEWORK::ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL; ++i)
			sendMemoryPool.push(/*new SendMemoryUnit()*/ std::move(new SendMemoryUnit()));

#ifdef _DEBUG
		ERROR_UTIL::ERROR_DISPLAY(L"[ERROR] SendPool의 적정 메모리가 부족해 추가할당하였습니다.");
#endif

#ifdef WONSY_LOG_MANAGER
		LogManager::GetInstance()->AddLog(LOG_TYPE::WARNING_LOG, L"sendMemoryPool의 적정 메모리가 부족해, ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL만큼 추가할당하였습니다. ");
#endif
		/*
			적정 메모리 풀 사이즈 측정.
		*/
		pRetMemoryUnit = new SendMemoryUnit();
	}
	return pRetMemoryUnit;
}

/*
	PushMemory()
		- Send가 끝난 메모리를 받아서 풀에 다시 집어넣습니다. 들어가세요.
	
	!0. 해당 함수 호출 이후, 인자로 전달된 파라미터의 사용은 금지됩니다. 
*/
void SendMemoryPool::PushMemory(SendMemoryUnit* pInMemoryUnit)
{
	sendMemoryPool.push(std::move(pInMemoryUnit));
}
