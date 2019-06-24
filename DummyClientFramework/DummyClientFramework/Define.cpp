#include "stdafx.h"
#include "Define.h"

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