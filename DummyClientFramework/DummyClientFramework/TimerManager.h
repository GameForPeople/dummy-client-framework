#pragma once

#include "Define.h"
#include "Custom.hh"

//---
#if USE_TIMER_MANAGER == 1
#define WONSY_TIMER_MANAGER
#endif
//---

#ifdef WONSY_TIMER_MANAGER
namespace WonSY
{
	/*
	TimerUnit
		- Ÿ�̸ӿ��� ���Ǵ� �޸� �����Դϴ�.
	*/

	struct TimerUnit
	{
		TIMER_TYPE timerType;
		_TimeType eventTime;
		_ClientIndexType ownerKey;	// �ش� Ÿ�̸Ӹ� ������ Ű.
		_ClientIndexType targetKey; // �ش� Ÿ�̸��� ������ �Ǵ� Ű.
		// std::any timerData;	// timerType�� ���� Data�� ����.
		_TimeDataType timerData;

	public:
		TimerUnit() noexcept;
		~TimerUnit();
	};

	struct TimerUnitCompareFunction
	{
		bool operator()(TimerUnit* left, TimerUnit* right) noexcept
		{
			return (left->eventTime) > (right->eventTime);
		}
	};

	class TimerManager /*: �̱��� */
	{
	public:
		_DO_NOT_DELETE _NODISCARD static inline TimerManager* GetInstance() noexcept { return TimerManager::instance; };

		static void MakeInstance(NetworkManager& networkManager) { TimerManager::instance = new TimerManager(networkManager); };

		static void DeleteInstance() { delete instance; }

	public:
		void TimerThread();
		void AddTimerEvent(const TIMER_TYPE, const _ClientIndexType ownerKey, const _ClientIndexType targetKey, const TIME waitTime, const _TimeDataType data);

	private:
		void ProcessTimerEvent_CUSTOM(TimerUnit*);

		_DO_NOT_DELETE _NODISCARD TimerUnit* PopTimerUnit();
		void PushTimerUnit(TimerUnit*);

	private:
		NetworkManager& networkManagerInstance;
		static inline TimerManager* instance = nullptr;

		TimerManager(NetworkManager& networkManagerInstance);
		~TimerManager();

		std::thread timerThread;
		_TimeType nowTime;

		//std::vector<concurrency::concurrent_queue<TimerUnit*>> timerCont;
		concurrency::concurrent_priority_queue<TimerUnit*, TimerUnitCompareFunction> timerCont;
		concurrency::concurrent_queue<TimerUnit*> timerMemoryPool;
	public:
		//concurrency::concurrent_queue<TimerUnit*>* GetTimerContWithIndex(const int inTimerContIndex);
	};
}
#endif