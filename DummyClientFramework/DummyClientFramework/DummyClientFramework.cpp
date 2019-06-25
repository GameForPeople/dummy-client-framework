#include "stdafx.h"
#include "NetworkManager.h"
#include "DummyClientFramework.h"

#include "Define.h"

DummyClientFramework::DummyClientFramework(/*NetworkManager* pInNetworkManager*/)
	: networkManager()
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
	for (auto& client : networkManager->clientArr)
	{
		if (client->isLogin)
		{
			
		}
	}
	//networkManager->clientArrLock.unlock_shared(); // -------------------- 0
}

void DummyClientFramework::Update()
{
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