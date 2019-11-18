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
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"sendMemoryPool�� �ʱ� �Ҵ� �������" + std::to_wstring(sendMemoryPool.unsafe_size()) + L" �Դϴ�.");
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
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"sendMemoryPool�� �Ҹ��ڰ� ȣ��Ǿ����ϴ�. ");
#endif
	sendMemoryPool.clear();
}

/*
	PopMemory()
		- Send�� �ʿ��� MemoryUnit�� ��û�մϴ�.
	?0. ������ �ʱ� �޸� Ǯ ����� �˱� ����, ���� SendǮ�� ���� ���, �α׸� ����մϴ�.
*/
SendMemoryUnit* SendMemoryPool::PopMemory()
{
	SendMemoryUnit* pRetMemoryUnit{ nullptr };

	/*while*/ if (!sendMemoryPool.try_pop(pRetMemoryUnit))
	{
		//assert(retMemoryUnit != nullptr, "retMemoryUnit�� try_pop�� nullptr�� ��ȯ�߽��ϴ�.");
		
		for (int i = 0; i < FRAMEWORK::ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL; ++i)
			sendMemoryPool.push(/*new SendMemoryUnit()*/ std::move(new SendMemoryUnit()));

#ifdef _DEBUG
		ERROR_UTIL::ERROR_DISPLAY(L"[ERROR] SendPool�� ���� �޸𸮰� ������ �߰��Ҵ��Ͽ����ϴ�.");
#endif

#ifdef WONSY_LOG_MANAGER
		LogManager::GetInstance()->AddLog(LOG_TYPE::WARNING_LOG, L"sendMemoryPool�� ���� �޸𸮰� ������, ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL��ŭ �߰��Ҵ��Ͽ����ϴ�. ");
#endif
		/*
			���� �޸� Ǯ ������ ����.
		*/
		pRetMemoryUnit = new SendMemoryUnit();
	}
	return pRetMemoryUnit;
}

/*
	PushMemory()
		- Send�� ���� �޸𸮸� �޾Ƽ� Ǯ�� �ٽ� ����ֽ��ϴ�. ������.
	
	!0. �ش� �Լ� ȣ�� ����, ���ڷ� ���޵� �Ķ������ ����� �����˴ϴ�. 
*/
void SendMemoryPool::PushMemory(SendMemoryUnit* pInMemoryUnit)
{
	sendMemoryPool.push(std::move(pInMemoryUnit));
}
