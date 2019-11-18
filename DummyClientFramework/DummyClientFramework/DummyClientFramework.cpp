#include "stdafx.h"
#include "NetworkManager.h"
#include "DummyClientFramework.h"

#include "Define.h"
#include "LogManager.h"
#include "TimerManager.h"

DummyClientFramework::DummyClientFramework(/*NetworkManager* pInNetworkManager*/)
	: hWnd()
	, networkManager()
{
	TestIntegrity();
#ifdef WONSY_LOG_MANAGER
	LogManager::MakeInstance();
#endif
}

DummyClientFramework::~DummyClientFramework()
{
#ifdef WONSY_LOG_MANAGER
	LogManager::GetInstance()->DeleteInstance();
#endif

	TimerManager::DeleteInstance();
	delete networkManager;
}

void DummyClientFramework::Create(HWND hWnd)
{
	this->hWnd = hWnd;

	printf("github.com/GameForPeople/dummy-client-framework \n");
	printf("\n■■■■■■■■■■■■■■■■■■■■■■■■■\n");
	printf("■ Dummy Client Framework ver 1.1 \n");
	printf("■		원성연 (KoreaGameMaker@gmail.com)\n");
	printf("■\n");
	printf("■ IP : %s \n", NETWORK::SERVER_IP.c_str());
	printf("■ Server Listen Port : %d \n", NETWORK::SERVER_PORT);
	printf("■■■■■■■■■■■■■■■■■■■■■■■■■\n");

	UNICODE_UTIL::SetLocaleForKorean();

	int tempUniqueKey{ - 1};
#if USE_CONTROLLED_CLIENT == __ON
	std::wcout << L"\n CONTROLLED_CLIENT의 UniqueKey값을 입력해주세요 (없으면 -1) :  ";
	std::wcin >> tempUniqueKey;
#endif
	//networkManager = std::make_unique<NetworkManager>(tempUniqueKey);
	networkManager = new NetworkManager(tempUniqueKey);

#ifdef WONSY_TIMER_MANAGER
	TimerManager::MakeInstance(*networkManager);
#endif

#ifdef WONSY_LOG_MANAGER
#ifdef DEFAULT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"DEFAULT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#elif HOTSPOT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"HOTSPOT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#endif
#endif
}

void DummyClientFramework::Draw(HDC hdc)
{
	int tempCount{ 0 };
	int zeroPosCount{ 0 };

	//NetworkManager::_ClientArrType tempClientArr;
	//{
	//	std::shared_lock<std::shared_mutex> tempSharedLock(networkManager->clientArrLock);
	//	tempClientArr = networkManager->clientArr;
	//}
	//
	//for (auto& pClient : tempClientArr)
	//{
	//	if (pClient->isLogin)
	//	{
	//		const _PosType tempX = static_cast<_PosType>((pClient->posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
	//		const _PosType tempY = static_cast<_PosType>(((pClient->posY - GAME::ZONE_MIN_Y) * (static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));
	//		
	//		Rectangle(hdc, tempX - GAME::ACTOR_X_SIZE, tempY - GAME::ACTOR_Y_SIZE, tempX + GAME::ACTOR_X_SIZE, tempY + GAME::ACTOR_Y_SIZE);
	//	}
	//}

#if USE_CONTROLLED_CLIENT == __ON
	if (networkManager->GetIsFindControlledClient())
	{
		HBRUSH	broadcastBrush, oldBrush;

		broadcastBrush = CreateSolidBrush(RGB(100, 100, 100));
		oldBrush = (HBRUSH)SelectObject(hdc, broadcastBrush);

		auto [posX, posY] = networkManager->GetControlledClientPosition();

		posX = static_cast<_PosType>((posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
		posY = static_cast<_PosType>((posY - GAME::ZONE_MIN_Y) * ((static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));

		Rectangle(hdc, posX - GAME::BROADCAST_X_RENDER_SIZE, posY - GAME::BROADCAST_Y_RENDER_SIZE,
			posX + GAME::BROADCAST_X_RENDER_SIZE, posY + GAME::BROADCAST_Y_RENDER_SIZE);

		SelectObject(hdc, oldBrush);
		DeleteObject(broadcastBrush);
	}
#endif

	for (const auto& pClient : networkManager->clientArr)
	{
		if (pClient->isLogin)
		{
			const _PosType tempX = static_cast<_PosType>((pClient->posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
			const _PosType tempY = static_cast<_PosType>(((pClient->posY - GAME::ZONE_MIN_Y) * (static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));
			
			Rectangle(hdc, tempX - GAME::ACTOR_X_SIZE, tempY - GAME::ACTOR_Y_SIZE, tempX + GAME::ACTOR_X_SIZE, tempY + GAME::ACTOR_Y_SIZE);
		}
	}
	
#if USE_CONTROLLED_CLIENT == __ON
	if (networkManager->GetIsFindControlledClient())
	{
		HBRUSH	redBrush, oldBrush;
		
		redBrush = CreateSolidBrush (RGB ( 255, 0, 0 ));
		oldBrush = (HBRUSH)SelectObject ( hdc, redBrush);
		
		auto [posX, posY] = networkManager->GetControlledClientPosition();
		
		posX = static_cast<_PosType>((posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
		posY = static_cast<_PosType>((posY - GAME::ZONE_MIN_Y) * ((static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));

		Rectangle(hdc, posX - GAME::ACTOR_X_SIZE * 1.5, posY - GAME::ACTOR_Y_SIZE * 1.5, posX + GAME::ACTOR_X_SIZE * 1.5, posY + GAME::ACTOR_Y_SIZE * 1.5);
		
		SelectObject ( hdc, oldBrush);
		DeleteObject (redBrush);
	}
#endif
	
	auto tempStr = (L"Dummy Count : " + std::to_wstring(networkManager->GetConnectedClientCount()));
	
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkColor(hdc, RGB(255, 255, 0));
	
	TextOutW(hdc, 0, 0, tempStr.c_str(), wcslen(tempStr.c_str())); // tempStr.length() or tempStr.size()
}

bool DummyClientFramework::Update()
{
	if (networkManager->ConnectWithinMaxClient())
	{
		// std::cout << "Connected Client Count :"<< networkManager->GetConnectedClientCount() << "\n";
		// return true;
	}
	else
	{
		// std::cout << "Connected Client Count : MAX - " << networkManager->GetConnectedClientCount() << "\n";
		// return false;
	}
	
	networkManager->ProcessUpdate_CUSTOM();
	return true;
}

void DummyClientFramework::InputKey(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	using namespace KEY;

	switch (iMessage)
	{
		case WM_KEYDOWN:
		{
			if (wParam == VK_P) {}
			if (wParam == VK_Q) { SendMessage(hWnd, WM_DESTROY, 0, 0); return; }
		} break;

		case WM_KEYUP:
		{
		} break;
	}
}