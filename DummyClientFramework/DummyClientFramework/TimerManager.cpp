#include "stdafx.h"

#include "TimerManager.h"

//---------------------------------------------------------------------------
// TimerUnit
//---------------------------------------------------------------------------
TimerUnit::TimerUnit()
	: timerType(TIMER_TYPE::NONE_TYPE), objectKey()
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
	nowTime(0), MAX_COOL_TIME(static_cast<int>(TIME::MAX_TIME) + 100),
	timerCont(),
	timerMemoryPool(),
	postQueuedFunctionCallCount(0)
{
	timerCont.reserve(MAX_COOL_TIME);
	for (int i = 0; i < MAX_COOL_TIME; ++i) { timerCont.emplace_back(); }
	
	for (int i = 0; i < TIME::PRE_MAKE_TIMER_UNIT_COUNT; ++i) { timerMemoryPool.push(new TimerUnit()); }

	SetPostQueuedFunctionCallCountAndTimerMemoryHeadCont(MAX_COOL_TIME);

	//std::wcout << L"!. TimerManager의 초기 할당 사이즈는 " << timerMemoryPool.unsafe_size() << L" 입니다." << std::endl;
}

TimerManager::~TimerManager()
{
	TimerUnit* retTimerUnit = nullptr;
	while (timerMemoryPool.try_pop(retTimerUnit)) { delete retTimerUnit; }

	for (auto& iter : timerCont)
	{
		while (iter.try_pop(retTimerUnit)) { delete retTimerUnit; }
	}
}

void TimerManager::SetPostQueuedFunctionCallCountAndTimerMemoryHeadCont(const BYTE inCallCount)
{
	postQueuedFunctionCallCount = inCallCount;

	timerMemoryHeadCont.reserve(inCallCount);
	for (int i = 0; i < inCallCount; ++i) { timerMemoryHeadCont.emplace_back(); }
}

DWORD WINAPI TimerManager::StartTimerThread()
{
	TimerManager::GetInstance()->TimerThread();

	return 0;
};

void TimerManager::TimerThread()
{
	assert(timerMemoryHeadCont.size(), L"TimerThread가 실행되었으나, 워커쓰레드 갯수가 할당되지 않았습니다. SetPostQueuedFunctionCallCountAndTimerMemoryHeadCont를 호출해주세요.");
	using namespace std::literals;

	while (7)
	{
		std::this_thread::sleep_for(100ms); // 정확히 0.1s Sleep은 아님.

		nowTime.load() != MAX_COOL_TIME - 1
			? nowTime.fetch_add(1)
			: nowTime = 0;

		const int tempInt = nowTime;

		if (const int tempContSize = static_cast<int>(timerCont[tempInt].unsafe_size())
			; tempContSize == 0)
		{

		}
		else if (tempContSize < 10)
		{
			if(PostQueuedCompletionStatus(hIOCP, 0, tempInt, reinterpret_cast<LPOVERLAPPED>(&timerMemoryHeadCont[0][tempInt % 10] /*+ sizeof(MEMORY_UNIT_TYPE)*/))
				== ERROR)
			{
				std::cout << WSAGetLastError() << std::endl;
				ERROR_HANDLING::ERROR_QUIT(L"PostQueuedCompletionStatus()");
			}
		}
		else
		{
			const int tempArrIndex = tempInt % 10;

			for (int i = 0; i < postQueuedFunctionCallCount; ++i)
			{
				if(PostQueuedCompletionStatus(hIOCP, 0, tempInt, reinterpret_cast<LPOVERLAPPED>(&timerMemoryHeadCont[i][tempArrIndex] /*+ sizeof(MEMORY_UNIT_TYPE)*/))
					== ERROR)
				{
					std::cout << WSAGetLastError() << std::endl;
					ERROR_HANDLING::ERROR_QUIT(L"PostQueuedCompletionStatus()");
				}
			}
		}
	}
}

// 성능적 단점이 있더라도, 실수 방지 위해, int -> TIME으로 변경.
void TimerManager::AddTimerEvent(TimerUnit* inTimerUnit, TIME waitTime)
{
	int tempWaitTime = static_cast<int>(waitTime);
	tempWaitTime += nowTime;

	if (tempWaitTime >= MAX_COOL_TIME) tempWaitTime -= MAX_COOL_TIME;

	timerCont[tempWaitTime].push(inTimerUnit);
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

concurrency::concurrent_queue<TimerUnit*>*
TimerManager::GetTimerContWithIndex(const int inTimerContIndex)
{
	return &(timerCont[inTimerContIndex]);
}