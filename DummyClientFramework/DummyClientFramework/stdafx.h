// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
//#define NOMINMAX
#include <windows.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS	// inet_addr

// C 런타임 헤더 파일입니다.
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>

#include <memory>
#include <cwchar>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <iostream>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "wininet.lib")
#include <WinSock2.h>

#include <atlimage.h>

#include <mutex>
#include <shared_mutex>
#include <atomic>

#include <array>
#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <concurrent_priority_queue.h>

#include <string>

#include <cassert>

#include <fstream>
#include <filesystem>

#include <chrono>

#include <any>

#define		_NORETURN			[[noreturn]]
#define		_NODISCARD			[[nodiscard]]
#define		_DEPRECATED			[[deprecated]]
#define		_MAYBE_UNUSED		[[maybe_unused]]
#define		_FALLTHROUGH		[[fallthrough]]

#include "magic_enum.hpp"

using namespace std::chrono;
