#include "stdafx.h"

#include "LogManager.h"

#include "Define.h"
#include "PerformanceManager.h"

//LogManager* LogManager::instance = nullptr;
//std::once_flag LogManager::instanceFlag;

LogManager::LogManager()
	: logFilePath("Log/DummyClientFramework_log_" + TIME_UTIL::GetCurrentDateTime() + ".txt")
	, logCont()
	, saveFlag(true)
{
	//logCont.reserve(50);
	UNICODE_UTIL::SetLocaleForKorean();
}

void LogManager::MakeInstance()
{
	std::call_once(instanceFlag, [/* void */]() noexcept(false) ->void
		{
			instance = new LogManager();
			if (instance == nullptr) throw ERROR;
		}
	);
}

void LogManager::DeleteInstance()
{
	if (ATOMIC_UTIL::T_CAS(&(instance->saveFlag), true, false))
	{
		instance->SaveLog();
		delete instance;
		//instance = nullptr;
	}
}

void LogManager::AddLog(const LOG_TYPE logType, const std::wstring_view& logString)
{
	logCont.push_back(std::make_pair(logType, std::move(static_cast<std::wstring>(logString))));

#ifdef _DEBUG
		switch (logType)
		{
		case LOG_TYPE::WARNING_LOG: { std::unique_lock<std::mutex>localLock(printLock); std::wcout << L"[WARNING] " << logString;	break; }
		case LOG_TYPE::ERROR_LOG: { std::unique_lock<std::mutex>localLock(printLock);  std::wcout << L"[ERROR] " << logString;		break; }
		case LOG_TYPE::DEV_LOG: { std::unique_lock<std::mutex>localLock(printLock);  std::wcout << L"[LOG] " << logString;		break; }
			default: 
			{ std::unique_lock<std::mutex>localLock(printLock); std::wcout << L"[???] " << logString; }
		}
#endif // _DEBUG
}

void LogManager::SaveLog()
{
	std::wofstream outFile(instance->logFilePath, std::ios::out);

	outFile << logFilePath.filename();
	outFile << "";
	outFile << "";
	outFile << "===========================================";

#ifdef PERFORMANCE_LOG //Use PerformanceManager?
	assert(PerformanceManager::GetInstance() != nullptr, L"PerformanceManager를 LogManager이전에 제거하지 마세요!");
	auto& resultCont = PerformanceManager::GetInstance()->GetResultCont();

	// Save Performance Log
	int index{ - 1};
	constexpr auto functionNameCont = magic_enum::enum_names<FUNCTION_NAME>();

	for (auto& functionResultCont : resultCont)
	{
		const size_t functionCallCount = [&functionResultCont]() noexcept -> const size_t 
		{ 
			if (auto contSize = functionResultCont.unsafe_size()) return contSize;
			return 1;
		}();

		PerformanceUnit::_ResultTimeType sumValue{};
		PerformanceUnit::_ResultTimeType retValue{};

		while (functionResultCont.try_pop(retValue))
		{
			sumValue += retValue;
		}

		sumValue /= functionCallCount;

		const std::string tempString = std::string(functionNameCont[++index]);
		outFile << L"함수 : " << UNICODE_UTIL::StringToWString(tempString)
				<< L" , 불린 횟수 : " << functionCallCount 
				<< L" , 평균 소요 시간 : " << sumValue.count();
	}

	outFile << "===========================================";
	outFile << "";
	outFile << "";
	outFile << "";

#endif

	// Save Log
	for (const auto& [logType, logData] : logCont)
	{
		switch (logType)
		{
			case LOG_TYPE::WARNING_LOG:	outFile << L"[WARNING] " << logData;	break;
			case LOG_TYPE::ERROR_LOG:	outFile << L"[ERROR] " << logData;		break;
			case LOG_TYPE::DEV_LOG:		outFile << L"[LOG] " << logData;		break;
			default: outFile << L"[???] " << logData;
		}
	}

	outFile << L"[GoodBye] 더미 클라이언트 프레임워크가 종료되었습니다. : "
		+ UNICODE_UTIL::StringToWString(TIME_UTIL::GetCurrentDateTime()) ;

	outFile.close();
}