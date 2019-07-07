#include "stdafx.h"
#include "Define.h"

#include "LogManager.h"

namespace ERROR_UTIL
{
	_NORETURN void ERROR_QUIT(const WCHAR *msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL
		);

		LogManager::GetInstance()->AddLog(LOG_TYPE::ERROR_LOG, (LPTSTR)lpMsgBuf);
		MessageBox(NULL, (LPTSTR)lpMsgBuf, msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(1);
	};
	
	void ERROR_DISPLAY(const WCHAR *msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		LogManager::GetInstance()->AddLog(LOG_TYPE::WARNING_LOG, (WCHAR*)lpMsgBuf);
		wprintf(L"[%s] %s", msg, (WCHAR*)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	void HandleRecvError()
	{
		if(auto retValue = WSAGetLastError(); 
			retValue == ERROR_IO_PENDING) return;
		else 
			ERROR_DISPLAY(((L"HandleRecvError() : " + std::to_wstring(retValue)).c_str()));
	};

	void HandleSendError()
	{
		if (auto retValue = WSAGetLastError();
			retValue == ERROR_IO_PENDING) return;
		else
			ERROR_DISPLAY(((L"HandleSendError() : " + std::to_wstring(retValue)).c_str()));
	};
}

namespace TIME_UTIL
{
	const std::string GetCurrentDateTime() {
		time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
		struct tm  tstruct;
		char       buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

		return buf;
	}
}

namespace UNICODE_UTIL
{
	void SetLocaleForKorean()
	{
		_wsetlocale(LC_ALL, L"Korean");

		// ? 계속 오류 내뱉어서 일단 꺼놓음.
		//26444 왜 때문에, 굳이 필요 없이, L-Value를 만들어야하는가;
		/*auto oldLocale = std::wcout.imbue(std::locale("koeran")); */
	}

	_NODISCARD std::string WStringToString(const std::wstring& InWString)
	{
		const int sizeBuffer = WideCharToMultiByte(CP_ACP, 0, &InWString[0], -1, NULL, 0, NULL, NULL);

		std::string retString(sizeBuffer, 0);

		WideCharToMultiByte(CP_ACP, 0, &InWString[0], -1, &retString[0], sizeBuffer, NULL, NULL);

		// FixError ==
		retString.pop_back(); //(retString.end(), retString.end());
		//retString.insert(retString.end(), '\0');

		return retString;
	}

	_NODISCARD std::wstring StringToWString(const std::string& InString)
	{
		const int sizeBuffer = MultiByteToWideChar(CP_ACP, 0, &InString[0], -1, NULL, 0);

		std::wstring retString(sizeBuffer, 0);

		MultiByteToWideChar(CP_ACP, 0, &InString[0], -1, &retString[0], sizeBuffer);

		return retString;
	}
}