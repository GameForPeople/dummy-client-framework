#include "stdafx.h"

#include "LogManager.h"

#include "Define.h"

LogManager* LogManager::instance = nullptr;
std::once_flag LogManager::instanceFlag;

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
	// 출력까지는 동기화 하지 않습니다.
	switch (logType)
	{
		case LOG_TYPE::WARNING_LOG:	std::wcout << L"[WARNING] " << logString;	break;
		case LOG_TYPE::ERROR_LOG:	std::wcout << L"[ERROR] " << logString;		break;
		case LOG_TYPE::DEV_LOG:		std::wcout << L"[LOG] " << logString;		break;
		default: std::wcout << L"[???] " << logString;
	}
#endif // _DEBUG
}

void LogManager::SaveLog()
{
	std::wofstream outFile(instance->logFilePath, std::ios::out);

	outFile << logFilePath.filename();

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