#pragma once

#include "Define.h"
#include "Custom.hh"

struct TimerMemoryHead;
/*
	TimerUnit
		- Ÿ�̸ӿ��� ���Ǵ� �޸� �����Դϴ�.
*/

enum class TIME : /*unsigned short*/ _TimeType
{
	SECOND = 1000,
	MINUTE = 60000,
};

enum class TIMER_TYPE : unsigned char
{
	NONE_TYPE,
	NPC_MOVE,
	NPC_ATTACK,

	PUSH_OLD_KEY
};

struct TimerUnit
{
	TIMER_TYPE timerType;	
	_TimeType eventTime;
	_ClientIndexType ownerKey;	// �ش� Ÿ�̸Ӹ� ������ Ű.
	_ClientIndexType targetKey; // �ش� Ÿ�̸��� ������ �Ǵ� Ű.
	// std::any timerData;	// timerType�� ���� Data�� ����.

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
	_NODISCARD static inline TimerManager* GetInstance() noexcept { return TimerManager::instance; };

	/*_NODISCARD*/ static void MakeInstance(HANDLE hIOCP) { TimerManager::instance = new TimerManager(hIOCP); };

	static void DeleteInstance() { delete instance; }

public:
	void TimerThread();
	static DWORD WINAPI StartTimerThread();
	void AddTimerEvent(const TIMER_TYPE, const _ClientIndexType ownerKey, const _ClientIndexType targetKey, const TIME waitTime);

private:
	void ProcessTimerEvent_CUSTOM(TimerUnit*);
	
	_NODISCARD TimerUnit* PopTimerUnit();
	void PushTimerUnit(TimerUnit*);

private:
	HANDLE hIOCP;
	static inline TimerManager* instance = nullptr;

	TimerManager(HANDLE hIOCP);
	~TimerManager();

	_TimeType nowTime;	// 
	
	//std::vector<concurrency::concurrent_queue<TimerUnit*>> timerCont;
	concurrency::concurrent_priority_queue<TimerUnit*, TimerUnitCompareFunction> timerCont;
	concurrency::concurrent_queue<TimerUnit*> timerMemoryPool;
public:
	//concurrency::concurrent_queue<TimerUnit*>* GetTimerContWithIndex(const int inTimerContIndex);
};