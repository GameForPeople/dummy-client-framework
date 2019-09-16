#include "stdafx.h"
#include "NetworkManager.h"
#include "DummyClientFramework.h"

#include "Define.h"
#include "LogManager.h"

DummyClientFramework::DummyClientFramework(/*NetworkManager* pInNetworkManager*/)
	: hWnd()
	, networkManager()
{
	TestIntegrity();
	LogManager::MakeInstance();
}

DummyClientFramework::~DummyClientFramework()
{
	LogManager::GetInstance()->DeleteInstance();
}

void DummyClientFramework::Create(HWND hWnd)
{
	this->hWnd = hWnd;

	std::cout << "\n Client를 실행중이라면, 해당 Client의 UniqueKey 값을 입력해주세요(없으면 -1) :  ";

	int tempUniqueKey{};
	std::cin >> tempUniqueKey;

	networkManager = std::make_unique<NetworkManager>(tempUniqueKey);

#ifdef DEFAULT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"DEFAULT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#elif HOTSPOT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"HOTSPOT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#endif
}

void DummyClientFramework::Draw(HDC hdc)
{
	int tempCount{ 0 };
	int zeroPosCount{ 0 };
	//networkManager->clientArrLock.lock_shared();	// +++++++++++++++++++ 1
	for (auto& pClient : networkManager->clientArr)
	{
		if (pClient->isLogin)
		{
			const _PosType tempX = static_cast<_PosType>((pClient->posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
			const _PosType tempY = static_cast<_PosType>(((pClient->posY - GAME::ZONE_MIN_Y) * (static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));
			
			Rectangle(hdc, tempX - GAME::ACTOR_X_SIZE, tempY - GAME::ACTOR_Y_SIZE, tempX + GAME::ACTOR_X_SIZE, tempY + GAME::ACTOR_Y_SIZE);
		}
	}
	//networkManager->clientArrLock.unlock_shared(); // -------------------- 0
	
	if (networkManager->GetIsFindControlledClient())
	{
		HBRUSH	redBrush, oldBrush;
		
		redBrush = CreateSolidBrush ( RGB ( 255, 0, 0 ) );
		oldBrush = (HBRUSH)SelectObject ( hdc, redBrush);
		
		auto [posX, posY] = networkManager->GetControlledClientPosition();
		
		posX = static_cast<_PosType>((posX - GAME::ZONE_MIN_X) * ((static_cast<float>(WINDOW::WINDOW_WIDTH) / GAME::ZONE_X_SIZE)));
		posY = static_cast<_PosType>((posY - GAME::ZONE_MIN_Y) * ((static_cast<float>(WINDOW::WINDOW_HEIGHT) / GAME::ZONE_Y_SIZE)));

		Rectangle(hdc, posX - GAME::ACTOR_X_SIZE * 2, posY - GAME::ACTOR_Y_SIZE * 2, posX + GAME::ACTOR_X_SIZE * 2, posY + GAME::ACTOR_Y_SIZE * 2);
		
		SelectObject ( hdc, oldBrush);
		DeleteObject (redBrush);
	}
	
	auto tempStr = (L"Dummy Count : " + std::to_wstring(networkManager->GetConnectedClientCount()));
	
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkColor(hdc, RGB(255, 255, 0));
	
	TextOutW(hdc, 0, 0, tempStr.c_str(), wcslen(tempStr.c_str())); // tempStr.length() or tempStr.size()
}

bool DummyClientFramework::Update()
{
	if (networkManager->ConnectWithinMaxClient())
	{
		std::cout << networkManager->GetConnectedClientCount() << "\n";
		networkManager->ProcessUpdate_CUSTOM();
		return true;
	}
	else
	{
		std::cout << "Max Client! \n";
		return false;
	}
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
	}
	break;

	case WM_KEYUP:
	{
	}
	break;
	}
}