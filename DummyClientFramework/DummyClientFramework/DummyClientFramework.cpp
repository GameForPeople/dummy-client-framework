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
	networkManager = std::make_unique<NetworkManager>();

#ifdef DEFAULT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"DEFAULT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#elif HOTSPOT_TEST_MODE
	LogManager::GetInstance()->AddLog(LOG_TYPE::DEV_LOG, L"HOTSPOT_TEST_MODE 더미 클라이언트가 정상적으로 실행되었습니다.");
#endif
}

void DummyClientFramework::Draw(HDC hdc)
{
	//networkManager->clientArrLock.lock_shared();	// +++++++++++++++++++ 1
	for (auto& pClient : networkManager->clientArr)
	{
		if (pClient->isLogin)
		{
			const _PosType tempX = static_cast<_PosType>((static_cast<float>(pClient->posX - GAME::ZONE_MIN_X) / GAME::ZONE_X_SIZE) * WINDOW::WINDOW_WIDTH);
			const _PosType tempY = static_cast<_PosType>((static_cast<float>(pClient->posY - GAME::ZONE_MIN_Y) / GAME::ZONE_Y_SIZE) * WINDOW::WINDOW_HEIGHT);

			std::cout << "tempX : " << tempX << ", " << "tempY : " << tempY << "\n";
			Rectangle(hdc, tempX, tempY, tempX + GAME::ACTOR_X_SIZE, tempY + GAME::ACTOR_Y_SIZE);
		}
	}
	//networkManager->clientArrLock.unlock_shared(); // -------------------- 0
	
	auto tempStr = (L"Dummy Count : " + std::to_wstring(networkManager->GetConnectedClientCount()));
	TextOutW(hdc, 50, 50, tempStr.c_str(), wcslen(tempStr.c_str())); // tempStr.length() or tempStr.size()
}

bool DummyClientFramework::Update()
{
	if (networkManager->ConnectWithinMaxClient())
	{
		std::cout << "true \n";
		networkManager->ProcessUpdate_CUSTOM();
		return true;
	}
	else
	{
		std::cout << "false \n";
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