#include "stdafx.h"
#include "NetworkManager.h"
#include "DummyClientFramework.h"

#include "Define.h"

DummyClientFramework::DummyClientFramework(/*NetworkManager* pInNetworkManager*/)
	: hWnd()
	, networkManager()
{
}

DummyClientFramework::~DummyClientFramework()
{
}

void DummyClientFramework::Create(HWND hWnd)
{
	this->hWnd = hWnd;
	networkManager = std::make_unique<NetworkManager>();
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

			Rectangle(hdc, tempX, tempY, tempX + GAME::ACTOR_X_SIZE, tempY + GAME::ACTOR_Y_SIZE);
		}
	}
	//networkManager->clientArrLock.unlock_shared(); // -------------------- 0
}

void DummyClientFramework::Update()
{
	networkManager->ConnectWithinMaxClient();
	networkManager->ProcessUpdate();
}

void DummyClientFramework::InputKey(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	using namespace KEY;

	switch (iMessage)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_Q)
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			return;
		}
	}
	break;

	case WM_KEYUP:
	{

	}
	break;
	}
}