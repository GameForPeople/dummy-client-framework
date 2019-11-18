#include "stdafx.h"

#include "NetworkManager.h"
#include "TimerManager.h"

#ifdef WONSY_TIMER_MANAGER
//---------------------------------------------------------------------------
// TimerUnit
//---------------------------------------------------------------------------
TimerUnit::TimerUnit() noexcept
	: timerType(TIMER_TYPE::NONE_TYPE)
	, eventTime()
	, ownerKey()
	, targetKey()
	, timerData()
{
}

TimerUnit::~TimerUnit()
{
}

//---------------------------------------------------------------------------
// TimerManager
//---------------------------------------------------------------------------

TimerManager::TimerManager(NetworkManager& networkManagerInstance) :
	networkManagerInstance(networkManagerInstance),
	nowTime(GetTickCount64()),
	timerCont(),
	timerMemoryPool(),
	timerThread()
{
	for (int i = 0; i < FRAMEWORK::PRE_ALLOCATION_TIMER_UNIT_COUNT; ++i) { timerMemoryPool.push(new TimerUnit()); }
	//std::wcout << L"!. TimerManager의 초기 할당 사이즈는 " << timerMemoryPool.unsafe_size() << L" 입니다." << std::endl;

	timerThread = std::thread([this]() {this->TimerThread(); });
}

TimerManager::~TimerManager()
{
	TimerUnit* retTimerUnit = nullptr;
	while (timerMemoryPool.try_pop(retTimerUnit)) { delete retTimerUnit; }
	while (timerCont.try_pop(retTimerUnit)) { delete retTimerUnit; }

	timerThread.join();
}

void TimerManager::TimerThread()
{
	using namespace std::literals;
	
	while (7)
	{
		std::this_thread::yield();
		const _TimeType tempNowTime = GetTickCount64();

		while (timerCont.size())
		{
			TimerUnit* retTimerUnit{ nullptr };
			
			while (!timerCont.try_pop(retTimerUnit))
			{
				break;
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
}

void TimerManager::AddTimerEvent(const TIMER_TYPE inTimerType, const _ClientIndexType ownerKey, const _ClientIndexType targetKey, const TIME waitTime, const _TimeDataType data)
{
	TimerUnit* timerUnit = PopTimerUnit();

	timerUnit->timerType = inTimerType;
	timerUnit->timerData = data;

	timerUnit->ownerKey = ownerKey;
	timerUnit->targetKey = targetKey;

	timerUnit->eventTime = GetTickCount64() + static_cast<_TimeType>(waitTime);

	timerCont.push(timerUnit);
}

TimerUnit* TimerManager::PopTimerUnit()
{
	TimerUnit* retTimerUnit = nullptr;

	return timerMemoryPool.try_pop(retTimerUnit)
		? retTimerUnit
		: []()->TimerUnit* { std::cout << "TimerUnit 이 부족해서 추가 할당을 하였습니다. " << std::endl; return new TimerUnit(); }();
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
#endif