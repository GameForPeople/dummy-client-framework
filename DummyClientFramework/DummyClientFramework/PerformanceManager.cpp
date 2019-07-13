#include "stdafx.h"

#include "PerformanceManager.h"

PerformanceManager* PerformanceManager::instance = nullptr;

PerformanceUnit::PerformanceUnit(const FUNCTION_NAME inFuntionName)
	: function(inFuntionName)
	, startTime(std::chrono::high_resolution_clock::now())
	, resultTime()
{
}

PerformanceUnit::~PerformanceUnit()
{
	PerformanceManager::GetInstance()->AddTimeResult(*this);
}

//-----------------------------------------------------------------
//	PerformanceManager
//-----------------------------------------------------------------
PerformanceManager::PerformanceManager()
	: resultCont()
{
}

void PerformanceManager::MakeInstance()
{
	instance = new PerformanceManager();
}

void PerformanceManager::DeleteInstance()
{
	delete instance;
}

std::unique_ptr<PerformanceUnit> PerformanceManager::StartTimer(const FUNCTION_NAME inFuntionName)
{
	return std::make_unique<PerformanceUnit>(inFuntionName);
}

void PerformanceManager::AddTimeResult(const PerformanceUnit& performanceUnit)
{
	resultCont[static_cast<int>(performanceUnit.function)].push
	(
		std::chrono::duration_cast<std::chrono::milliseconds>
		(
			std::chrono::high_resolution_clock::now() - performanceUnit.startTime
		)
	);
}

PerformanceManager::_ResultContType& PerformanceManager::GetResultCont()
{
	return resultCont;
}

