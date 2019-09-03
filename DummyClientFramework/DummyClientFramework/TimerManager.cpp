#include "stdafx.h"

#include "TimerManager.h"

//---------------------------------------------------------------------------
// TimerUnit
//---------------------------------------------------------------------------
TimerUnit::TimerUnit()
	: timerType(TIMER_TYPE::NONE_TYPE), eventTime(),
	ownerKey(), targetKey()
{
}

TimerUnit::~TimerUnit()
{
}

//---------------------------------------------------------------------------
// TimerManager
//---------------------------------------------------------------------------

TimerManager::TimerManager(HANDLE hIOCP) :
	hIOCP(hIOCP),
	nowTime(GetTickCount64()),
	timerCont(),
	timerMemoryPool()
{
	for (int i = 0; i < FRAMEWORK::PRE_ALLOCATION_TIMER_UNIT_COUNT; ++i) { timerMemoryPool.push(new TimerUnit()); }
	//std::wcout << L"!. TimerManager�� �ʱ� �Ҵ� ������� " << timerMemoryPool.unsafe_size() << L" �Դϴ�." << std::endl;
}

TimerManager::~TimerManager()
{
	TimerUnit* retTimerUnit = nullptr;
	while (timerMemoryPool.try_pop(retTimerUnit)) { delete retTimerUnit; }
	while (timerCont.try_pop(retTimerUnit)) { delete retTimerUnit; }
}

DWORD WINAPI TimerManager::StartTimerThread()
{
	TimerManager::GetInstance()->TimerThread();

	return 0;
};

void TimerManager::TimerThread()
{
	using namespace std::literals;
	//std::this_thread::sleep_for(1ms); 
	const _TimeType tempNowTime = GetTickCount64();

	while (timerCont.size())
	{
		TimerUnit* retTimerUnit;
		while (!timerCont.try_pop(retTimerUnit))
		{
		}

		if (tempNowTime < retTimerUnit->eventTime)
		{
			timerCont.push(retTimerUnit);
			break;
		}
		
		ProcessTimerEvent_CUSTOM(retTimerUnit);
		PushTimerUnit(retTimerUnit);
	}
}

void TimerManager::AddTimerEvent(TimerUnit* inTimerUnit, TIME waitTime)
{
	const _TimeType tempWaitTime = GetTickCount64() + static_cast<_TimeType>(waitTime);
	inTimerUnit->eventTime = tempWaitTime;

	timerCont.push(inTimerUnit);
}

TimerUnit* TimerManager::PopTimerUnit()
{
	TimerUnit* retTimerUnit = nullptr;

	return timerMemoryPool.try_pop(retTimerUnit)
		? retTimerUnit
		: []()->TimerUnit* { std::cout << "TimerUnit �� �����ؼ� �߰� �Ҵ��� �Ͽ����ϴ�. " << std::endl; return new TimerUnit(); }();
}

void TimerManager::PushTimerUnit(TimerUnit* inTimerUnit)
{
	timerMemoryPool.push(inTimerUnit);
}

//concurrency::concurrent_queue<TimerUnit*>*
//TimerManager::GetTimerContWithIndex(const int inTimerContIndex)
//{
//	return &(timerCont[inTimerContIndex]);
//}